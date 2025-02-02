#include "Exception.hpp"
#include "Trace.hpp"

#include <Windows.h>

static void process_attach();

static void process_detach();

BOOL APIENTRY DllMain([[maybe_unused]] HINSTANCE hInstance,
                      [[maybe_unused]] DWORD dwReason,
                      [[maybe_unused]] LPVOID lpReserved)
{
	try
	{
		if (dwReason == DLL_PROCESS_ATTACH)
		{
			process_attach();
		}
		if (dwReason == DLL_PROCESS_DETACH)
		{
			process_detach();
		}
		return TRUE;
	}
	catch (const WinApiException& ex)
	{
		TRACE("uncaught WinApiException with code ", ex.code(), " and error ", ex.error())
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
		TRACE("uncaught unknown or critical exception")
	}
	return FALSE;
}

void process_attach()
{
	TRACE(L"PROCESS ATTACH")
}

void process_detach()
{
	TRACE(L"PROCESS DETACH")
}
