#include "Handlers/CallDllGenericProcedureHandler.hpp"

#include "Commands/CallDllGenericProcedureCommand.hpp"

CallDllGenericProcedureHandler::CallDllGenericProcedureHandler(std::shared_ptr<LibrariesContainer> libraries):
	m_libraries(std::move(libraries))
{
}

std::vector<IProduct::Ptr> CallDllGenericProcedureHandler::do_handle(const ICommand::Ptr& command)
{
	const auto call_dll_generic_procedure_handler = std::dynamic_pointer_cast<CallDllGenericProcedureCommand>(command);
	Buffer result = m_libraries->call(
		call_dll_generic_procedure_handler->m_library_id,
		call_dll_generic_procedure_handler->m_ordinal,
		call_dll_generic_procedure_handler->m_parameters
	);
	return {std::make_unique<RawProduct>(std::move(result))};
}
