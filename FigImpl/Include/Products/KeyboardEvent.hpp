#pragma once
#include <cstdint>
#include <vector>

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

using KeyboardEvents = std::vector<KeyboardEvent>;
