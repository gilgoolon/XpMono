#include "Synchronization/IWaitable.hpp"

static WaitStatus to_wait_status(const DWORD result)
{
	switch (result)
	{
	case WAIT_OBJECT_0:
		return WaitStatus::FINISHED;

	case WAIT_ABANDONED:
		return WaitStatus::OBJECT_CLOSED;

	case WAIT_TIMEOUT:
		return WaitStatus::TIMEOUT;

	case WAIT_FAILED:
		[[fallthrough]];
	default:
		return WaitStatus::FAILED;
	}
}

WaitStatus IWaitable::wait(const Time::Duration timeout) const
{
	const DWORD result = WaitForSingleObject(handle(), timeout.count());
	return to_wait_status(result);
}

void IWaitable::sleep(const Time::Duration duration)
{
	Sleep(Time::duration_cast<Time::Millis>(duration).count());
}
