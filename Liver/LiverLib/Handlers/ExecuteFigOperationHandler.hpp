#pragma once
#include "FigOperationsFetcher.hpp"
#include "FigsContainer.hpp"
#include "ICommandHandler.hpp"

class ExecuteFigOperationHandler final : public ICommandHandler
{
public:
	explicit ExecuteFigOperationHandler(std::shared_ptr<FigsContainer> figs,
	                                    std::shared_ptr<FigOperationsContainer> fig_operations_container);
	~ExecuteFigOperationHandler() override = default;
	ExecuteFigOperationHandler(const ExecuteFigOperationHandler&) = delete;
	ExecuteFigOperationHandler& operator=(const ExecuteFigOperationHandler&) = delete;
	ExecuteFigOperationHandler(ExecuteFigOperationHandler&&) = delete;
	ExecuteFigOperationHandler& operator=(ExecuteFigOperationHandler&&) = delete;

	[[nodiscard]] std::vector<IProduct::Ptr> do_handle(const ICommand::Ptr& command) override;

private:
	std::shared_ptr<FigsContainer> m_figs;
	std::shared_ptr<FigOperationsContainer> m_fig_operations_container;
};
