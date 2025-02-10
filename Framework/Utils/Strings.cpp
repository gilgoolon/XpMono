#include "Utils/Strings.hpp"

#include <system_error>
#include <Windows.h>

#include "Exception.hpp"

std::string Strings::to_string(const std::wstring& str)
{
	if (str.empty())
	{
		return {};
	}

	const int size_needed = WideCharToMultiByte(
		CP_UTF8,
		0,
		str.c_str(),
		static_cast<int>(str.size()),
		nullptr,
		0,
		nullptr,
		nullptr
	);
	if (size_needed == 0)
	{
		throw WinApiException(ErrorCode::FAILED_STRING_CONVERSION);
	}

	std::string utf8_str(size_needed, '\0');
	const int bytes_written = WideCharToMultiByte(
		CP_UTF8,
		0,
		str.c_str(),
		static_cast<int>(str.size()),
		utf8_str.data(),
		size_needed,
		nullptr,
		nullptr
	);
	if (bytes_written == 0)
	{
		throw WinApiException(ErrorCode::FAILED_STRING_CONVERSION);
	}

	return utf8_str;
}

std::wstring Strings::to_wstring(const BSTR& str)
{
	return str;
}

Buffer Strings::to_buffer(const std::wstring& str)
{
	const auto* const start = reinterpret_cast<const uint8_t*>(str.data());
	return Buffer(start, start + str.size() * sizeof(std::wstring::value_type));
}

std::wstring Strings::to_wstring(const Buffer& buffer)
{
	const auto* const start = reinterpret_cast<const std::wstring::value_type*>(buffer.data());
	return std::wstring(start, start + buffer.size() / sizeof(std::wstring::value_type));
}

std::wstring Strings::to_wstring(const std::string& str)
{
	if (str.empty())
	{
		return {};
	}

	const int size_needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		static_cast<int>(str.size()),
		nullptr,
		0
	);
	if (size_needed == 0)
	{
		throw WinApiException(ErrorCode::FAILED_STRING_CONVERSION);
	}

	std::wstring utf16_str(size_needed, '\0');
	const int bytes_written = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		static_cast<int>(str.size()),
		utf16_str.data(),
		size_needed
	);
	if (bytes_written == 0)
	{
		throw WinApiException(ErrorCode::FAILED_STRING_CONVERSION);
	}
	return utf16_str;
}

std::wstring Strings::join(const std::vector<std::wstring>& strings, wchar_t separator)
{
	if (strings.empty())
	{
		return L"";
	}

	uint32_t total_size = 0;
	for (const auto& s : strings)
	{
		total_size += s.size();
	}
	total_size += strings.size() - 1;

	std::wstring result;
	result.reserve(total_size);

	for (uint32_t i = 0; i < strings.size(); ++i)
	{
		result += strings[i];
		if (i != strings.size() - 1)
		{
			result += separator;
		}
	}
	return result;
}

std::vector<std::wstring> Strings::parse_raw_strings(const std::wstring& raw_strings)
{
	std::vector<std::wstring> result;
	for (const wchar_t* cur = raw_strings.c_str();
	     *cur != L'\0';
	     cur += wcsnlen_s(cur, raw_strings.size()) + 1)
	{
		result.emplace_back(cur);
	}
	return result;
}
