#include "FigOperationsFetcher.hpp"

#include "Exception.hpp"
#include "Products/ErrorProduct.hpp"
#include "Products/FigOperationErrorProduct.hpp"
#include "Products/FigProduct.hpp"
#include "Products/RawProduct.hpp"
#include "Synchronization/Event.hpp"

FigOperationsFetcher::FigOperationsFetcher(Event::Ptr quit_event,
                                           std::shared_ptr<ProductsContainer> products,
                                           std::shared_ptr<FigOperationsContainer> operations_container):
	m_quit_event(std::move(quit_event)),
	m_is_quit(false),
	m_operations(std::move(operations_container)),
	m_products(std::move(products))
{
}

void FigOperationsFetcher::run()
{
	while (!m_is_quit)
	{
		m_operations->handle_operations(
			[this](std::vector<FigOperationsContainer::CommandLinkedFigOperation>& operations)
			{
				operations_handler(operations);
			}
		);
	}
}

std::vector<std::shared_ptr<IWaitable>> FigOperationsFetcher::get_iteration_triggers(
	const std::vector<FigOperationsContainer::CommandLinkedFigOperation>& operations) const
{
	std::vector<std::shared_ptr<IWaitable>> events;
	events.push_back(m_quit_event);
	events.push_back(m_operations->m_notifier);
	for (const FigOperationsContainer::CommandLinkedFigOperation& operation : operations)
	{
		events.push_back(operation.fig_operation->m_event);
	}
	return events;
}

void FigOperationsFetcher::fetch_operations(std::vector<FigOperationsContainer::CommandLinkedFigOperation>& operations)
{
	auto iterator = operations.begin();

	while (!operations.empty() && iterator != operations.end())
	{
		const FigOperationsContainer::CommandLinkedFigOperation& operation = *iterator;

		const FigModule::StatusResult status = operation.fig_operation->status();
		switch (status.execution_status)
		{
		case Fig::ExecutionStatus::EXECUTING:
			[[fallthrough]];
		case Fig::ExecutionStatus::EXECUTING_CAN_TAKE:
			++iterator;
			continue;

		case Fig::ExecutionStatus::FAILED:
		{
			std::vector<IProduct::Ptr> products;
			products.push_back(
				std::make_unique<FigOperationErrorProduct>(
					operation.linked_command,
					operation.fig_operation->m_id,
					operation.fig_operation->m_module->id(),
					status.fig_specific_code
				)
			);
			m_products->insert_all(std::move(products));
			operations.erase(iterator);
			continue;
		}

		case Fig::ExecutionStatus::FINISHED:
			break;

		default:
			throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
		}

		std::vector<IProduct::Ptr> products;
		products.push_back(
			std::make_unique<FigProduct>(
				operation.linked_command,
				operation.fig_operation->m_module->id(),
				operation.fig_operation->m_id,
				operation.fig_operation->take_all()
			)
		);
		m_products->insert_all(std::move(products));

		operations.erase(iterator);
	}
}

void FigOperationsFetcher::operations_handler(
	std::vector<FigOperationsContainer::CommandLinkedFigOperation>& operations)
{
	static constexpr Time::Duration ITERATION_DELAY = Time::Seconds(5);

	const std::vector<std::shared_ptr<IWaitable>> triggers = get_iteration_triggers(operations);
	const WaitResult wait_result = IWaitable::wait_for_any(triggers, ITERATION_DELAY);

	if (wait_result.status == WaitStatus::TIMEOUT)
	{
		return;
	}

	if ((wait_result.status == WaitStatus::FINISHED && triggers[*wait_result.triggered_object].get() == m_quit_event
			.get()) ||
		wait_result.status != WaitStatus::FINISHED)
	{
		m_is_quit = true;
		return;
	}

	if (triggers[*wait_result.triggered_object].get() == m_operations->m_notifier.get())
	{
		m_operations->m_notifier->unset();
		return;
	}

	fetch_operations(operations);
}
