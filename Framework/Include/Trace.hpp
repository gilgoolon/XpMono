#pragma once
#include "Debug.hpp"
#include "Utils/Formatting.hpp"

#include <filesystem>

#define TRACE(...) \
    try { \
        debug(Formatting::format(std::filesystem::path(__FILE__).filename(), ":", __LINE__, " " ,__FUNCTION__, " - ", __VA_ARGS__, L"\n")); \
    } catch (...) \
	{debug(L"failed to trace");};
