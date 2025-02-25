#pragma once
#include <cstdint>
#include <string>

namespace Ip
{
[[nodiscard]] uint32_t parse_ip(const std::string& ip);
};
