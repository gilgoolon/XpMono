#pragma once
#include "FigOperation.hpp"
#include "FigOperationsContainer.hpp"
#include "ProductsContainer.hpp"
#include "Processes/IRunner.hpp"

class FigOperationsFetcher final : public IRunner
{
public:
	explicit FigOperationsFetcher(Event::Ptr quit_event,
	                              std::shared_ptr<ProductsContainer> products,
	                              std::shared_ptr<FigOperationsContainer> operations_container);
	~FigOperationsFetcher() override = default;
	FigOperationsFetcher(const FigOperationsFetcher&) = delete;
	FigOperationsFetcher& operator=(const FigOperationsFetcher&) = delete;
	FigOperationsFetcher(FigOperationsFetcher&&) = delete;
	FigOperationsFetcher& operator=(FigOperationsFetcher&&) = delete;

	void run() override;

	friend class FigsContainer;

private:
	Event::Ptr m_quit_event;
	bool m_is_quit;
	std::shared_ptr<FigOperationsContainer> m_operations;
	std::shared_ptr<ProductsContainer> m_products;

	[[nodiscard]] std::vector<std::shared_ptr<IWaitable>> get_iteration_triggers(
		const std::vector<FigOperationsContainer::CommandLinkedFigOperation>& operations) const;

	void fetch_operations(std::vector<FigOperationsContainer::CommandLinkedFigOperation>& operations);

	void operations_handler(
		std::vector<FigOperationsContainer::CommandLinkedFigOperation>& operations);
};
