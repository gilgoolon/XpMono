#include "Protections/UnhandledExceptionFilter.hpp"

#include "Trace.hpp"

Protections::UnhandledExceptionFilter::UnhandledExceptionFilter():
	m_previous_filter(SetUnhandledExceptionFilter(filter))
{
}

Protections::UnhandledExceptionFilter::~UnhandledExceptionFilter()
{
	try
	{
		SetUnhandledExceptionFilter(m_previous_filter);
	}
	catch (...)
	{
		TRACE(L"failed to set previous unhandled exception filter")
	}
}

LONG Protections::UnhandledExceptionFilter::filter([[maybe_unused]] EXCEPTION_POINTERS* const exception)
{
	TRACE(L"CRITICAL EXCEPTION: uncaught top level exception with code ", exception->ExceptionRecord->ExceptionCode);
	return EXCEPTION_EXECUTE_HANDLER;
}
