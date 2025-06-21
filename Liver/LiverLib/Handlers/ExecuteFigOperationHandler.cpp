#include "Handlers/ExecuteFigOperationHandler.hpp"

#include "Commands/ExecuteFigOperationCommand.hpp"

ExecuteFigOperationHandler::ExecuteFigOperationHandler(std::shared_ptr<FigsContainer> figs,
                                                       std::shared_ptr<FigOperationsContainer>
                                                       fig_operations_container):
	m_figs(std::move(figs)),
	m_fig_operations_container(std::move(fig_operations_container))
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

	m_fig_operations_container->consume(std::move(operation), command);

	return {};
}
