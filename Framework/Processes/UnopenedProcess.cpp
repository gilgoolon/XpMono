#include "Processes/UnopenedProcess.hpp"

UnopenedProcess::UnopenedProcess(const uint32_t pid):
	m_pid(pid)
{
}

uint32_t UnopenedProcess::get_pid() const
{
	return m_pid;
}

Process::Ptr UnopenedProcess::open() const
{
	return std::make_unique<Process>(m_pid);
}

bool UnopenedProcess::is_ignored() const
{
	static constexpr uint32_t IDLE_PROCESS_PID = 0;
	return m_pid == IDLE_PROCESS_PID;
}
