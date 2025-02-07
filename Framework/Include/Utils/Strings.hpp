#pragma once
#include "Buffer.hpp"

#include <span>
#include <string>

namespace Strings
{
[[nodiscard]] std::string to_string(const std::wstring& str);

[[nodiscard]] Buffer to_buffer(const std::wstring& str);

[[nodiscard]] std::wstring to_wstring(const Buffer& buffer);

[[nodiscard]] std::wstring to_wstring(const std::string& buffer);

template <typename Buffer, typename... Buffers>
std::vector<typename Buffer::value_type> concat(const Buffer& first, const Buffers&... rest)
{
	using ValueType = typename Buffer::value_type;
	std::vector<std::span<const ValueType>> spans = {std::span(first), std::span(rest)...};

	std::size_t total_size = 0;
	for (const auto& span : spans)
	{
		total_size += span.size();
	}

	std::vector<ValueType> result(total_size);

	auto dest = result.begin();
	for (const auto& span : spans)
	{
		dest = std::copy(span.begin(), span.end(), dest);
	}

	return result;
}
}
