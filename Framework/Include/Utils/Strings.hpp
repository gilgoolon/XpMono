#pragma once
#include "Buffer.hpp"
#include "Time.hpp"

#include <span>
#include <string>

namespace Strings
{
[[nodiscard]] std::string to_string(const std::wstring& str);

[[nodiscard]] std::wstring to_wstring(const BSTR& str);

[[nodiscard]] Buffer to_buffer(const std::wstring& str);

[[nodiscard]] std::wstring to_wstring(const Buffer& buffer);

[[nodiscard]] std::wstring to_wstring(const std::string& buffer);

template <typename Buffer, typename... Buffers>
Buffer concat(const Buffer& first, const Buffers&... rest)
{
	uint32_t total_size = first.size() + (rest.size() + ...);

	Buffer result = first;
	result.reserve(total_size);
	(result.insert(result.end(), rest.begin(), rest.end()), ...);

	return result;
}

[[nodiscard]] std::wstring join(const std::vector<std::wstring>& strings, wchar_t separator);

template <typename T>
[[nodiscard]] std::wstring to_wstring(const T& value)
{
	std::wostringstream out;
	out << value;
	return out.str();
}

template <typename T>
[[nodiscard]] std::wstring to_wstring(const std::vector<T>& arr)
{
	static constexpr std::wstring_view PREFIX = L"[";
	static constexpr std::wstring_view SUFFIX = L"]";
	static constexpr wchar_t SEPARATOR = L',';
	std::vector<std::wstring> values;
	for (const T& value : arr)
	{
		values.push_back(Strings::to_wstring<T>(value));
	}
	return std::wstring{PREFIX} + join(values, SEPARATOR) + std::wstring{SUFFIX};
}

template <>
[[nodiscard]] inline std::wstring to_wstring<BOOLEAN>(const BOOLEAN& value)
{
	return value == FALSE ? L"False" : L"True";
}

template <>
[[nodiscard]] inline std::wstring to_wstring<Time::Datetime>(const Time::Datetime& value)
{
	return to_wstring(Time::to_string(value));
}

[[nodiscard]] std::vector<std::wstring> parse_raw_strings(const std::wstring& raw_strings);
}
