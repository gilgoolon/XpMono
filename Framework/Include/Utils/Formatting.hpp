#pragma once
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace Formatting
{
template <typename... Args>
std::wstring format(Args... args)
{
	std::wostringstream oss;
	((oss << args), ...);
	return oss.str();
}

using Fields = std::vector<std::pair<std::wstring, std::optional<std::wstring>>>;

std::wstring format_fields(const std::vector<std::pair<std::wstring, std::optional<std::wstring>>>& fields);
}
