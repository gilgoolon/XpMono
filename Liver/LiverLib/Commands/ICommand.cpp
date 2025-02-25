#include "Commands/ICommand.hpp"

#include "Utils/Random.hpp"

ICommand::ICommand(const Id command_id):
	m_id(command_id)
{
}

ICommand::ICommand():
	ICommand(Random::generate<Id>())
{
}

ICommand::Id ICommand::id() const
{
	return m_id;
}
