#include "Commands/ExecuteFigOperationCommand.hpp"

ExecuteFigOperationCommand::ExecuteFigOperationCommand(const Id command_id,
                                                       const Fig::FigId fig_id,
                                                       const Fig::OperationType operation_type,
                                                       Buffer operation_parameters):
	ICommand(command_id),
	m_fig_id(fig_id),
	m_operation_type(operation_type),
	m_operation_parameters(std::move(operation_parameters))
{
}

ICommand::Type ExecuteFigOperationCommand::type() const
{
	return Type::EXECUTE_FIG_OPERATION;
}
