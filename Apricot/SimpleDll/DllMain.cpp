#include "Exception.hpp"
#include "Trace.hpp"
#include "Protections/EntryPointProtector.hpp"

#include <Windows.h>

class Exception;

BOOL WINAPI DllMain([[maybe_unused]] HINSTANCE instance,
                    [[maybe_unused]] const DWORD reason,
                    [[maybe_unused]] DWORD reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		MessageBoxW(nullptr, L"SimpleDll loaded", L"SimpleDll", 0);
	}
	if (reason == DLL_PROCESS_DETACH)
	{
		MessageBoxW(nullptr, L"SimpleDll unloaded", L"SimpleDll", 0);
	}
	return TRUE;
}

void throwing_function()
{
	try
	{
		Protections::EntryPointProtector protector;
		TRACE(L"in throwing function ");
		throw WinApiException(ErrorCode::UNKNOWN_ERROR);
	}
	catch ([[maybe_unused]] const Exception& exception)
	{
		TRACE(L"caught exception with code: ", exception.code());
		MessageBoxW(nullptr, L"SimpleDll::throwing_function", L"SimpleDll", 0);
	}
}

void throwing_uncaught_function()
{
	throw Exception(ErrorCode::UNKNOWN_ERROR);
}

void throwing_uncaught_seh_function()
{
	uint8_t* ptr = nullptr;
	*ptr = 1;
}

void throwing_seh_function()
{
	__try
	{
		uint8_t* ptr = nullptr;
		*ptr = 1;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

void exported_function_by_name()
{
	MessageBoxW(nullptr, L"SimpleDll::exported_function_by_name", L"SimpleDll", 0);
}

void exported_function_by_ordinal1()
{
	MessageBoxW(nullptr, L"SimpleDll::exported_function_by_ordinal1", L"SimpleDll", 0);
}
