#include "Processes/Thread.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

#include <process.h>

Thread::Thread(IRunner::Ptr runner):
	m_handle(create_thread(std::move(runner)))
{
}

uint32_t Thread::thread_main(void* const argument)
{
	try
	{
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
	static constexpr void* DEFAULT_SECURITY = nullptr;
	static constexpr unsigned DEFAULT_STACK_SIZE = 0;
	static constexpr unsigned RUN_ON_CREATION = 0;
	static constexpr unsigned* DONT_OUT_TID = nullptr;
	const uintptr_t result = _beginthreadex(
		DEFAULT_SECURITY,
		DEFAULT_STACK_SIZE,
		thread_main,
		runner.get(),
		RUN_ON_CREATION,
		DONT_OUT_TID
	);
	if (result == 0)
	{
		throw WinApiException(ErrorCode::FAILED_THREAD_CREATE);
	}
	runner.release();
	return reinterpret_cast<HANDLE>(result);
}
