#include "Utils/Sizes.hpp"

uint64_t Sizes::make_large_int(const uint32_t low_part, const uint32_t high_part)
{
	return (static_cast<uint64_t>(high_part) << 32) | low_part;
}
