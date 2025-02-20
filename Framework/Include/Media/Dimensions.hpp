#pragma once
#include <cstdint>

namespace Dimensions
{
struct Dimensions final
{
	int32_t left_bound;
	int32_t top_bound;
	int32_t right_bound;
	int32_t bottom_bound;
};

[[nodiscard]] Dimensions get_dimensions();
};
