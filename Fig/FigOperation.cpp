#include "FigOperation.hpp"

#include "Exception.hpp"
#include "FigException.hpp"

FigOperation::FigOperation(std::shared_ptr<FigModule> module, const Fig::OperationId id, const HANDLE unmanaged_event):
	m_module(std::move(module)),
	m_id(id),
	m_event(std::make_shared<UnmanagedEvent>(unmanaged_event))
{
}

Buffer FigOperation::take()
{
	return m_module->take(m_id);
}

FigModule::StatusResult FigOperation::status() const
{
	return m_module->status(m_id);
}

FigModule::StatusResult FigOperation::wait() const
{
	static constexpr Time::Duration OPERATION_ITERATION_TIMEOUT = Time::Minutes(1);
	switch (m_event->wait(OPERATION_ITERATION_TIMEOUT))
	{
	case WaitStatus::TIMEOUT:
		throw Exception(ErrorCode::WAIT_TIMED_OUT);
	case WaitStatus::FINISHED:
		break;
	case WaitStatus::FAILED:
		[[fallthrough]];
	case WaitStatus::OBJECT_CLOSED:
		[[fallthrough]];
	default:
		throw WinApiException(ErrorCode::FAILED_WAIT);
	}
	return status();
}
