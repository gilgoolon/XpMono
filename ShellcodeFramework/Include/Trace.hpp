#pragma once
#include <Windows.h>

#ifdef _DEBUG

#define TRACE(message) OutputDebugStringW(__FILEW__ L":" __FUNCTIONW__ L": " message L"\n")

#else

#define TRACE(message) ;

#endif
