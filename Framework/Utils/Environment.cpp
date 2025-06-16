#include "Utils/Environment.hpp"

#include "Exception.hpp"

#include <Windows.h>

std::wstring Environment::expand_variables(const std::wstring& string)
{
	static constexpr wchar_t NULL_TERMINATOR = L'\0';
	std::wstring expanded_path(MAX_PATH, NULL_TERMINATOR);

	const DWORD result = ExpandEnvironmentStringsW(string.c_str(), expanded_path.data(), expanded_path.size());
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_ENVIRONMENT_EXPAND);
	}

	return {expanded_path.data(), expanded_path.data() + static_cast<SSIZE_T>(result) - 1};
}

std::filesystem::path Environment::get_temp_folder()
{
	static constexpr wchar_t NULL_TERMINATOR = L'\0';
	std::wstring buffer(MAX_PATH, NULL_TERMINATOR);
	const DWORD result = GetTempPath2W(buffer.size(), buffer.data());

	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_GET_TEMP_PATH);
	}

	return {buffer.data(), buffer.data() + static_cast<SSIZE_T>(result) - 1};
}
