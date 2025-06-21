#pragma once
#include "WlanClient.hpp"

namespace Wireless
{
[[nodiscard]] std::vector<Interface> enumerate_interfaces();
[[nodiscard]] std::vector<Network> enumerate_networks();
}
