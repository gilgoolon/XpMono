#include "Synchronization/Mutex.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

Mutex::Mutex(const std::wstring& name):
	m_handle(create_mutex(name))
{
	switch (wait(Time::INSTANT))
	{
	case WaitStatus::FINISHED:
	{
		break;
	}

	case WaitStatus::FAILED:
	{
		throw WinApiException(ErrorCode::FAILED_MUTEX_ACQUIRE);
	}

	case WaitStatus::OBJECT_CLOSED:
		[[fallthrough]];
	case WaitStatus::TIMEOUT:
		[[fallthrough]];
	default:
	{
		throw Exception(ErrorCode::FAILED_MUTEX_ACQUIRE);
	}
	}
}

Mutex::~Mutex()
{
	try
	{
		if (ReleaseMutex(m_handle.get()) == FALSE)
		{
			TRACE(L"failed to release mutex");
		}
	}
	catch (...)
	{
		TRACE(L"failed to release mutex");
	}
}

HANDLE Mutex::create_mutex(const std::wstring& name)
{
	static constexpr LPSECURITY_ATTRIBUTES DEFAULT_SECURITY = nullptr;
	static constexpr BOOL TAKE_OWNERSHIP = TRUE;
	const HANDLE result = CreateMutexW(DEFAULT_SECURITY, TAKE_OWNERSHIP, name.c_str());
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_MUTEX_CREATE);
	}
	return result;
}

HANDLE Mutex::handle() const
{
	return m_handle.get();
}
