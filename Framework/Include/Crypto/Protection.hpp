#pragma once
#include "Utils/Buffer.hpp"

namespace Crypto
{
[[nodiscard]] Buffer unprotect_data(const Buffer& data);
}
