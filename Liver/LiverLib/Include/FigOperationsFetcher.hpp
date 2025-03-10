#pragma once
#include "FigOperation.hpp"
#include "ProductsContainer.hpp"
#include "Processes/IRunner.hpp"
#include "Synchronization/CriticalSection.hpp"

class FigOperationsFetcher final : public IRunner
{
public:
	explicit FigOperationsFetcher(Event::Ptr quit_event, std::shared_ptr<ProductsContainer> products);
	~FigOperationsFetcher() override = default;
	FigOperationsFetcher(const FigOperationsFetcher&) = delete;
	FigOperationsFetcher& operator=(const FigOperationsFetcher&) = delete;
	FigOperationsFetcher(FigOperationsFetcher&&) = delete;
	FigOperationsFetcher& operator=(FigOperationsFetcher&&) = delete;

	void run() override;

	void consume(std::unique_ptr<FigOperation> operation, ICommand::Ptr command);

	friend class FigsContainer;

private:
	struct CommandLinkedFigOperation final
	{
		std::unique_ptr<FigOperation> fig_operation;
		ICommand::Ptr linked_command;
	};

	CriticalSection m_lock;
	Event::Ptr m_quit_event;
	Event::Ptr m_operation_notifier;
	std::vector<CommandLinkedFigOperation> m_operations;
	std::shared_ptr<ProductsContainer> m_products;

	[[nodiscard]] std::vector<std::shared_ptr<IWaitable>> get_iteration_triggers() const;

	void perform_iteration();
};
