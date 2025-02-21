#include "Commands/ICommand.hpp"

ICommand::ICommand(const Id command_id):
	m_id(command_id)
{
}

ICommand::Id ICommand::id() const
{
	return m_id;
}
