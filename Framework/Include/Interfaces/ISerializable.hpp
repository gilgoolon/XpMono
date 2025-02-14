#pragma once
#include "Utils/Buffer.hpp"

class ISerializable
{
public:
	explicit ISerializable() = default;
	virtual ~ISerializable() = default;
	ISerializable(const ISerializable&) = delete;
	ISerializable& operator=(const ISerializable&) = delete;
	ISerializable(ISerializable&&) = delete;
	ISerializable& operator=(ISerializable&&) = delete;

	[[nodiscard]] virtual Buffer serialize() const = 0;
};
