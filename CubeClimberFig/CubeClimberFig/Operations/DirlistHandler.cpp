#include "DirlistHandler.hpp"

#include "Trace.hpp"

DirlistHandler::DirlistHandler(std::unique_ptr<Event> operation_event):
	m_operation_event(std::move(operation_event)),
	m_status(Fig::ExecutionStatus::EXECUTING),
	m_code(Fig::FIG_SPECIFIC_CODE_RESERVED),
	m_data{}
{
}

Buffer DirlistHandler::take([[maybe_unused]] const uint32_t max_size)
{
	return m_data;
}

Fig::ExecutionStatus DirlistHandler::status()
{
	return m_status;
}

Fig::FigSpecificCode DirlistHandler::specific_code()
{
	return m_code;
}

void DirlistHandler::run()
{
	Time::sleep(Time::Seconds(10));
	m_operation_event->set();
	m_status = Fig::ExecutionStatus::FINISHED;
	TRACE(L"empty run");
}
