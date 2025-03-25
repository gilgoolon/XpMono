#pragma once
#include <cstdint>

struct KeyboardEvent
{
	enum class Type : uint8_t
	{
		UP = 0,
		DOWN = 1
	};

	Type type;
	uint32_t key_code;
};
