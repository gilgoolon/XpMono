#include "CommandsExecutor.hpp"

#include <algorithm>

CommandsExecutor::CommandsExecutor(std::shared_ptr<IWaitable> quit_event,
                                   std::shared_ptr<CommandsContainer> commands_container,
                                   Consumer consumer):
	m_quit_event(std::move(quit_event)),
	m_commands_container(std::move(commands_container)),
	m_consumer(std::move(consumer))
{
}

void CommandsExecutor::run()
{
	static constexpr Time::Seconds ITERATION_TIMEOUT{1};
	do
	{
		if (!m_commands_container->has_new())
		{
			continue;
		}

		ICommand::Ptr command = m_commands_container->pop();

		m_consumer(command);
	}
	while (m_quit_event->checked_wait(ITERATION_TIMEOUT) != WaitStatus::FINISHED);
}

void CommandsExecutor::consume(const std::vector<ICommand::Ptr>& commands)
{
	m_commands_container->insert_all(commands);
}
