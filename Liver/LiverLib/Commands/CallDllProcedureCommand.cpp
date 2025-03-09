#include "Commands/CallDllProcedureCommand.hpp"

CallDllProcedureCommand::CallDllProcedureCommand(const Id command_id,
                                                 const uint32_t library_id,
                                                 const uint16_t ordinal):
	ICommand(command_id),
	m_library_id(library_id),
	m_ordinal(ordinal)
{
}

ICommand::Type CallDllProcedureCommand::type() const
{
	return Type::CALL_DLL_PROCEDURE;
}
