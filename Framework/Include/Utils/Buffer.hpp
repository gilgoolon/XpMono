#pragma once
#include <cstdint>
#include <vector>

using Buffer = std::vector<uint8_t>;
using Buffers = std::vector<Buffer>;

inline uint32_t align_up(const uint32_t value, const uint32_t alignment)
{
	return value - (value % alignment) + alignment;
}

template <typename Type>
Buffer as_buffer(const Type& value)
{
	const auto begin = reinterpret_cast<const uint8_t*>(&value);
	return Buffer(begin, begin + sizeof(Type));
}
