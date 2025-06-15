#pragma once
#include <string>

namespace Environment
{
[[nodiscard]] std::wstring expand_variables(const std::wstring& string);
}
