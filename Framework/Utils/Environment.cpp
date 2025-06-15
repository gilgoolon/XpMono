#include "Utils/Environment.hpp"

#include "Exception.hpp"

#include <Windows.h>

std::wstring Environment::expand_variables(const std::wstring& string)
{
	static constexpr wchar_t NULL_TERMINATOR = L'\0';
	std::wstring expanded_path{MAX_PATH, NULL_TERMINATOR};

	const DWORD result = ExpandEnvironmentStringsW(string.c_str(), expanded_path.data(), expanded_path.size());
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_ENVIRONMENT_EXPAND);
	}

	expanded_path.resize(result);
	return expanded_path;
}
