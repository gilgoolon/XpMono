#include "LoadDllHandler.hpp"

LoadDllHandler::LoadDllHandler(ICommand::Ptr command):
	m_command(std::dynamic_pointer_cast<LoadDllCommand>(std::move(command)))
{
}

std::vector<IProduct::Ptr> LoadDllHandler::handle(LiverContext context) const
{
	return {};
}
