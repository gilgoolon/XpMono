#include "Commands/CallDllGenericProcedureCommand.hpp"

CallDllGenericProcedureCommand::CallDllGenericProcedureCommand(const Id command_id,
                                                               const uint32_t library_id,
                                                               const uint16_t ordinal,
                                                               Buffer parameters):
	ICommand(command_id),
	m_library_id(library_id),
	m_ordinal(ordinal),
	m_parameters(std::move(parameters))
{
}

ICommand::Type CallDllGenericProcedureCommand::type() const
{
	return Type::CALL_DLL_GENERIC_PROCEDURE;
}
