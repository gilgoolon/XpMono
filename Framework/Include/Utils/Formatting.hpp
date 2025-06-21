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
	oss << std::hex << std::setw(sizeof(Integer) * 2ul) << std::setfill(L'0') << value;
	return oss.str();
}
}
