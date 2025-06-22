#pragma once
#include "Debug.hpp"
#include "Exception.hpp"
#include "Macros.hpp"
#include "Com/ComException.hpp"
#include "Utils/Formatting.hpp"

#include <filesystem>

#ifndef __PROJECTW__
#define __PROJECTW__ L"???"
#endif

#ifdef _DEBUG

#define TRACE(...) \
    MACRO_START() \
	try \
	{ \
	        debug(Formatting::format(L"[" __PROJECTW__ L"] ", std::filesystem::path(__FILE__).filename().wstring(), ":", __LINE__, " ", __FUNCTION__, " - ", __VA_ARGS__, L"\n")); \
	} \
	catch (...) \
	{debug(L"failed to trace"); } \
	MACRO_END()

#define TRACE_S(str) \
	MACRO_START() \
	debug(L"[" __PROJECTW__ L"] " __FILEW__ L":" __FUNCTIONW__ L" - " str); \
	MACRO_END()

#else

#define TRACE(...) ;

#define TRACE_S(str) ;

#endif

#define CATCH_AND_TRACE() \
	catch ([[maybe_unused]] const WsaException& ex) \
	{ \
		TRACE("uncaught WsaException with code ", ex.code(), " and error ", ex.wsa_code()); \
	} \
	catch ([[maybe_unused]] const ComException& ex) \
	{\
		TRACE("uncaught ComException with code ", ex.code(), " and message ", ex.message().c_str()); \
	} \
	catch ([[maybe_unused]] const WinApiException& ex) \
	{\
		TRACE("uncaught WinApiException with code ", ex.code(), " and error ", ex.error()); \
	} \
	catch ([[maybe_unused]] const Exception& ex) \
	{ \
		TRACE("uncaught Exception with code ", ex.code()); \
	} \
	catch ([[maybe_unused]] const CriticalException&) \
	{ \
		TRACE("uncaught CriticalException"); \
	} \
	catch ([[maybe_unused]] const std::exception& ex) \
	{ \
		TRACE("uncaught std::exception: ", ex.what()); \
	} \
	catch (...) \
	{ \
		TRACE("uncaught unknown or critical exception"); \
	}
