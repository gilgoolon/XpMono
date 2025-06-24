#include "SafeTrace.hpp"

#include <Windows.h>

void SafeTrace::debug(const wchar_t* message)
{
	OutputDebugStringW(message);
}
