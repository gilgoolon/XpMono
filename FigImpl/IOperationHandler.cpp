#include "IOperationHandler.hpp"

IOperationHandler::IOperationHandler(std::unique_ptr<Event> operation_event):
	m_lock(),
	m_event(std::move(operation_event)),
	m_result{},
	m_status(Fig::ExecutionStatus::EXECUTING),
	m_specific_code(Fig::FIG_SPECIFIC_CODE_RESERVED)
{
}

Buffer IOperationHandler::take(const uint32_t max_size)
{
	const CriticalSection::Acquired acquired = m_lock.acquire();
	const uint32_t taken_size = min(max_size, m_result.size());
	Buffer result = {m_result.begin(), m_result.begin() + static_cast<int32_t>(taken_size)};
	if (result.empty() && m_status == Fig::ExecutionStatus::EXECUTING_CAN_TAKE)
	{
		m_status = Fig::ExecutionStatus::EXECUTING;
	}
	m_result.erase(m_result.begin(), m_result.begin() + static_cast<int32_t>(taken_size));
	return result;
}

Fig::ExecutionStatus IOperationHandler::status()
{
	return m_status;
}

Fig::FigSpecificCode IOperationHandler::specific_code()
{
	return m_specific_code;
}

void IOperationHandler::error(const Fig::FigSpecificCode specific_code)
{
	const auto acquired = m_lock.acquire();
	m_status = Fig::ExecutionStatus::FAILED;
	m_specific_code = specific_code;
	m_event->set();
}

void IOperationHandler::finished()
{
	const auto acquired = m_lock.acquire();
	m_status = Fig::ExecutionStatus::FINISHED;
	m_event->set();
}

void IOperationHandler::append(const Buffer& data)
{
	const auto acquired = m_lock.acquire();
	m_result.insert(m_result.end(), data.begin(), data.end());
	static constexpr uint32_t SIZE_TO_NOTIFY_TAKE = 4096;
	if (m_result.size() >= SIZE_TO_NOTIFY_TAKE)
	{
		m_status = Fig::ExecutionStatus::EXECUTING_CAN_TAKE;
		m_event->set();
	}
}
