#include "Utils/Strings.hpp"

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

std::string Strings::to_string(const Buffer& buffer)
{
	const auto* const start = reinterpret_cast<const std::string::value_type*>(buffer.data());
	return {start, start + buffer.size() / sizeof(std::string::value_type)};
}

std::wstring Strings::to_wstring(const BSTR& str)
{
	return str;
}

std::wstring Strings::to_wstring(const Buffer& buffer)
{
	const auto* const start = reinterpret_cast<const std::wstring::value_type*>(buffer.data());
	return {start, start + buffer.size() / sizeof(std::wstring::value_type)};
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

std::wstring Strings::precise_to_wstring(const double value, const size_t precision)
{
	std::wostringstream stream;
	stream << std::setprecision(precision) << value;
	return stream.str();
}

std::wstring Strings::join(const std::vector<std::wstring>& strings, const wchar_t separator)
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

std::vector<std::string> Strings::split(const std::string& string, const char delimiter)
{
	std::vector<std::string> result;
	std::string::size_type start = 0;
	std::string::size_type end;

	while ((end = string.find(delimiter, start)) != std::string::npos)
	{
		result.emplace_back(string.substr(start, end - start));
		start = end + 1;
	}
	result.emplace_back(string.substr(start));
	return result;
}

std::vector<std::wstring> Strings::split(const std::wstring& string, const wchar_t delimiter)
{
	std::vector<std::wstring> result;
	std::wstring::size_type start = 0;
	std::wstring::size_type end;

	while ((end = string.find(delimiter, start)) != std::wstring::npos)
	{
		result.emplace_back(string.substr(start, end - start));
		start = end + 1;
	}
	result.emplace_back(string.substr(start));
	return result;
}

uint8_t Strings::parse_uint8(const std::string& string)
{
	if (string.empty())
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}

	std::size_t idx = 0;

	static constexpr uint32_t DECIMAL_BASE = 10;
	const uint32_t value = std::stoul(string, &idx, DECIMAL_BASE);

	if (idx != string.size())
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}
	if (value > (std::numeric_limits<uint8_t>::max)())
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}

	return static_cast<uint8_t>(value);
}
