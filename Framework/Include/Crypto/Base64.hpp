#pragma once
#include "Utils/Buffer.hpp"

#include <string>

namespace Base64
{
[[nodiscard]] Buffer decode(const std::string& str64);
[[nodiscard]] std::string encode(const Buffer& data);
};
