#include "Handlers/LoadDllHandler.hpp"

#include "Liver.hpp"

LoadDllHandler::LoadDllHandler(ICommand::Ptr command):
	m_command(std::dynamic_pointer_cast<LoadDllCommand>(std::move(command)))
{
}

std::vector<IProduct::Ptr> LoadDllHandler::handle(LiverContext context) const
{
	context.libraries.load(m_command->m_library_id, m_command->m_dll_buffer);
	return {};
}
