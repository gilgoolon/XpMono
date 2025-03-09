#include "ExecuteFigOperationHandler.hpp"

#include "Commands/ExecuteFigOperationCommand.hpp"

ExecuteFigOperationHandler::ExecuteFigOperationHandler(std::unique_ptr<FigsContainer> figs):
	m_figs(std::move(figs))
{
}

std::vector<IProduct::Ptr> ExecuteFigOperationHandler::do_handle(const ICommand::Ptr& command)
{
	const auto execute_fig_operation_command = std::dynamic_pointer_cast<ExecuteFigOperationCommand>(command);
	m_figs->execute(
		execute_fig_operation_command->m_fig_id,
		execute_fig_operation_command->m_operation_type,
		execute_fig_operation_command->m_operation_parameters
	);
	return {};
}
