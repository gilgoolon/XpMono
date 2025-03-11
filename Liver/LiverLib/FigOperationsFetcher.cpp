#include "FigOperationsFetcher.hpp"

#include "Exception.hpp"
#include "Products/ErrorProduct.hpp"
#include "Products/FigOperationErrorProduct.hpp"
#include "Products/RawProduct.hpp"
#include "Synchronization/Event.hpp"

FigOperationsFetcher::FigOperationsFetcher(Event::Ptr quit_event, std::shared_ptr<ProductsContainer> products) :
	m_lock(),
	m_quit_event(std::move(quit_event)),
	m_operation_notifier(std::make_shared<Event>(Event::Type::AUTO_RESET)),
	m_operations(),
	m_products(std::move(products))
{
}

void FigOperationsFetcher::run()
{
	static constexpr Time::Duration ITERATION_DELAY = Time::Seconds(30);

	while (true)
	{
		const auto acquired = m_lock.acquire();

		const std::vector<std::shared_ptr<IWaitable>> triggers = get_iteration_triggers();
		const WaitResult wait_result = IWaitable::wait_for_any(triggers, ITERATION_DELAY);

		if (wait_result.status == WaitStatus::TIMEOUT)
		{
			continue;
		}

		if ((wait_result.status == WaitStatus::FINISHED && triggers[*wait_result.triggered_object].get() == m_quit_event
				.get()) ||
			wait_result.status != WaitStatus::FINISHED)
		{
			break;
		}

		if (triggers[*wait_result.triggered_object].get() == m_operation_notifier.get())
		{
			continue;
		}

		perform_iteration();
	}
}

void FigOperationsFetcher::consume(std::unique_ptr<FigOperation> operation, ICommand::Ptr command)
{
	m_operation_notifier->set();

	const auto acquired = m_lock.acquire();

	m_operations.emplace_back(std::move(operation), std::move(command));
}

std::vector<std::shared_ptr<IWaitable>> FigOperationsFetcher::get_iteration_triggers() const
{
	std::vector<std::shared_ptr<IWaitable>> events;
	events.push_back(m_quit_event);
	events.push_back(m_operation_notifier);
	for (const CommandLinkedFigOperation& operation : m_operations)
	{
		events.push_back(operation.fig_operation->m_event);
	}
	return events;
}

void FigOperationsFetcher::perform_iteration()
{
	auto iterator = m_operations.begin();

	while (iterator != m_operations.end())
	{
		const CommandLinkedFigOperation& operation = *iterator;

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
			m_operations.erase(iterator);
			continue;
		}

		case Fig::ExecutionStatus::FINISHED:
			break;

		default:
			throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
		}

		std::vector<IProduct::Ptr> products;
		products.push_back(std::make_unique<RawProduct>(operation.linked_command, operation.fig_operation->take()));
		m_products->insert_all(std::move(products));

		m_operations.erase(iterator);
	}
}
