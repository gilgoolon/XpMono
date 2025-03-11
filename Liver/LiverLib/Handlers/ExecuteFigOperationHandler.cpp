#include "ExecuteFigOperationHandler.hpp"

#include "Commands/ExecuteFigOperationCommand.hpp"

ExecuteFigOperationHandler::ExecuteFigOperationHandler(std::shared_ptr<FigsContainer> figs,
                                                       std::shared_ptr<FigOperationsFetcher> fig_operations_fetcher):
	m_figs(std::move(figs)),
	m_fig_operations_fetcher(std::move(fig_operations_fetcher))
{
}

std::vector<IProduct::Ptr> ExecuteFigOperationHandler::do_handle(const ICommand::Ptr& command)
{
	const auto execute_fig_operation_command = std::dynamic_pointer_cast<ExecuteFigOperationCommand>(command);
	std::unique_ptr<FigOperation> operation = m_figs->execute(
		execute_fig_operation_command->m_fig_id,
		execute_fig_operation_command->m_operation_type,
		execute_fig_operation_command->m_operation_parameters
	);

	m_fig_operations_fetcher->consume(std::move(operation), command);

	return {};
}
