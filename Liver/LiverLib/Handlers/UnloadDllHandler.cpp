#include "Handlers/UnloadDllHandler.hpp"

#include "Commands/UnloadDllCommand.hpp"

UnloadDllHandler::UnloadDllHandler(std::shared_ptr<LibrariesContainer> libraries):
	m_libraries(std::move(libraries))
{
}

std::vector<IProduct::Ptr> UnloadDllHandler::do_handle(const ICommand::Ptr& command)
{
	const auto load_dll_command = std::dynamic_pointer_cast<UnloadDllCommand>(command);
	m_libraries->unload(load_dll_command->m_library_id);
	return {};
}
