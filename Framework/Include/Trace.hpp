#pragma once
#include "Debug.hpp"
#include "Utils/Formatting.hpp"

#include <filesystem>

#ifndef __PROJECTW__
#define __PROJECTW__ L"???"
#endif

#ifdef _DEBUG

#define TRACE(...) \
    try { \
        debug(Formatting::format(L"[" __PROJECTW__ L"] ", std::filesystem::path(__FILE__).filename().wstring(), ":", __LINE__, " " ,__FUNCTION__, " - ", __VA_ARGS__, L"\n")); \
    } catch (...) \
	{debug(L"failed to trace");};

#define TRACE_S(str) debug(L"[" __PROJECTW__ L"] " __FILEW__ L":" __FUNCTIONW__ L" - " str);

#else

#define TRACE(...) ;

#define TRACE_S(str) ;

#endif
