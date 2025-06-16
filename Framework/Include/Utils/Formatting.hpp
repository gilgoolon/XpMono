#pragma once
#include <iomanip>
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

template <typename Integer>
std::wstring format_hex(Integer value)
{
	std::wostringstream oss;
	oss << std::uppercase << std::hex << std::setw(sizeof(Integer) / 2) << std::setfill(L'0') << value;
	return oss.str();
}

using Fields = std::vector<std::pair<std::wstring, std::optional<std::wstring>>>;

std::wstring format_fields(const std::vector<std::pair<std::wstring, std::optional<std::wstring>>>& fields);
}
