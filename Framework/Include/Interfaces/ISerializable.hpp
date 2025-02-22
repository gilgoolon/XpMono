#pragma once
#include "Utils/Buffer.hpp"

class ISerializable
{
public:
	explicit ISerializable() = default;
	virtual ~ISerializable() = default;

	[[nodiscard]] virtual Buffer serialize() const = 0;
};
