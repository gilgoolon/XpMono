#include "Debug.hpp"

#include <Windows.h>

void debug(const std::wstring& str)
{
	OutputDebugStringW(str.c_str());
}
