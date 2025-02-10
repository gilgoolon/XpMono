#include "Processes/Thread.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Protections/EntryPointProtector.hpp"

#include <process.h>

Thread::Thread(IRunner::Ptr runner):
	m_handle(create_thread(std::move(runner)))
{
}

DWORD Thread::thread_main(const LPVOID argument)
{
	try
	{
		Protections::EntryPointProtector protections;
		const IRunner::Ptr runner(static_cast<IRunner*>(argument));
		runner->run();
		return EXIT_SUCCESS;
	}
	catch (const WinApiException& ex)
	{
		TRACE("uncaught WinApiException with code ", ex.code(), " and error ", ex.error())
	}
	catch (const WinApiNtException& ex)
	{
		TRACE("uncaught WinApiExceptionNt with code ", ex.code(), " and status ", ex.status())
	}
	catch (const Exception& ex)
	{
		TRACE("uncaught Exception with code ", ex.code())
	}
	catch (const CriticalException&)
	{
		TRACE(L"uncaught critical exception")
	}
	catch (const std::exception& ex)
	{
		TRACE("uncaught std::exception: ", ex.what())
	}
	catch (...)
	{
		TRACE("uncaught unknown exception: ")
	}
	return EXIT_FAILURE;
}

HANDLE Thread::create_thread(IRunner::Ptr runner)
{
	static constexpr LPSECURITY_ATTRIBUTES DEFAULT_SECURITY = nullptr;
	static constexpr DWORD DEFAULT_STACK_SIZE = 0;
	static constexpr DWORD RUN_ON_CREATION = 0;
	static constexpr LPDWORD DONT_OUT_TID = nullptr;
	const HANDLE result = CreateThread(
		DEFAULT_SECURITY,
		DEFAULT_STACK_SIZE,
		thread_main,
		runner.get(),
		RUN_ON_CREATION,
		DONT_OUT_TID
	);
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_THREAD_CREATE);
	}
	runner.release();
	return result;
}
