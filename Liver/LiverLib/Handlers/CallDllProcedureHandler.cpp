#include "Handlers/CallDllProcedureHandler.hpp"

#include "Commands/CallDllProcedureCommand.hpp"

CallDllProcedureHandler::CallDllProcedureHandler(std::shared_ptr<LibrariesContainer> libraries):
	m_libraries(std::move(libraries))
{
}

std::vector<IProduct::Ptr> CallDllProcedureHandler::do_handle(const ICommand::Ptr& command)
{
	const auto call_dll_procedure_command = std::dynamic_pointer_cast<CallDllProcedureCommand>(command);
	m_libraries->call(call_dll_procedure_command->m_library_id, call_dll_procedure_command->m_ordinal);
	return {};
}
