#pragma once
#include <filesystem>
#include <string>

namespace Environment
{
[[nodiscard]] std::wstring expand_variables(const std::wstring& string);

[[nodiscard]] std::filesystem::path get_temp_folder();
}
