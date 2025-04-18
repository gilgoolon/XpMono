#pragma once

#include <string>
#include <vector>

namespace Strings
{
    std::string as_string(const std::vector<uint8_t>& buffer);

    std::vector<std::string> split(const std::string& str, const char delimiter);
}
