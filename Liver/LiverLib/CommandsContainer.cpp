#include "CommandsContainer.hpp"

void CommandsContainer::insert_all(const std::vector<ICommand::Ptr>& commands)
{
	auto lock = m_lock.acquire();
	m_commands.insert(m_commands.end(), commands.begin(), commands.end());
}

ICommand::Ptr CommandsContainer::pop()
{
	ICommand::Ptr command = nullptr;

	{
		auto lock = m_lock.acquire();
		command = *m_commands.begin();
		m_commands.erase(m_commands.begin());
	}

	return command;
}

bool CommandsContainer::has_new() const
{
	return !m_commands.empty();
}
