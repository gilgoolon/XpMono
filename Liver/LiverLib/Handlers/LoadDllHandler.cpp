#include "Handlers/LoadDllHandler.hpp"

#include "Liver.hpp"

LoadDllHandler::LoadDllHandler(std::shared_ptr<LibrariesContainer> libraries):
	m_libraries(std::move(libraries))
{
}

std::vector<IProduct::Ptr> LoadDllHandler::do_handle(const ICommand::Ptr& command)
{
	const auto load_dll_command = std::dynamic_pointer_cast<LoadDllCommand>(command);
	m_libraries->load(load_dll_command->m_library_id, load_dll_command->m_dll_buffer);
	return {};
}
