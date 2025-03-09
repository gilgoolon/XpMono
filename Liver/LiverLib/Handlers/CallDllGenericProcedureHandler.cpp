#include "Handlers/CallDllGenericProcedureHandler.hpp"

#include "Commands/CallDllGenericProcedureCommand.hpp"
#include "Products/RawProduct.hpp"

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

	std::vector<IProduct::Ptr> products;
	products.push_back(std::make_unique<RawProduct>(command, std::move(result)));
	return products;
}
