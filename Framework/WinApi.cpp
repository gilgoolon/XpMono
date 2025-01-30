#include "Include/WinApi.hpp"

static WinApi::WaitStatus result_to_wait_status(const DWORD result)
{
	switch (result)
	{
	case WAIT_OBJECT_0:
		return WinApi::WaitStatus::FINISHED;

	case WAIT_ABANDONED:
		return WinApi::WaitStatus::OBJECT_CLOSED;

	case WAIT_TIMEOUT:
		return WinApi::WaitStatus::TIMEOUT;

	case WAIT_FAILED:
		[[fallthrough]];
	default:
		return WinApi::WaitStatus::FAILED;
	}
}

WinApi::WaitStatus WinApi::wait(const HANDLE handle, const Time::Duration timeout)
{
	const DWORD result = WaitForSingleObject(handle, timeout.count());
	return result_to_wait_status(result);
}

void WinApi::sleep(const Time::Duration duration)
{
	Sleep(Time::duration_cast<Time::Millis>(duration).count());
}
