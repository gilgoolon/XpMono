#include <Windows.h>

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

void exported_function_by_name()
{
	MessageBoxW(nullptr, L"SimpleDll::exported_function_by_name", L"SimpleDll", 0);
}

void exported_function_by_ordinal1()
{
	MessageBoxW(nullptr, L"SimpleDll::exported_function_by_ordinal1", L"SimpleDll", 0);
}
