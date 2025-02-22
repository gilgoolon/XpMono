#pragma once
#include "Utils/Buffer.hpp"

class ISerializable
{
public:
	explicit ISerializable() = default;
	virtual ~ISerializable() = default;

	[[nodiscard]] virtual Buffer serialize() const = 0;

	template <typename T>
	[[nodiscard]] static Buffer serialize(T value)
	{
		return {reinterpret_cast<const uint8_t*>(value), reinterpret_cast<const uint8_t*>(value) + sizeof(T)};
	}
};
