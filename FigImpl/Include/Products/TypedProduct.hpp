#pragma once
#include "Interfaces/ISerializable.hpp"

#include <cstdint>

class TypedProduct : public ISerializable
{
public:
	enum class Type : uint32_t
	{
		RESERVED = 0,
		TEXT,
		RAW,
		PICTURE_BNP,
		PICTURE_PNG,
	};

	explicit TypedProduct() = default;
	~TypedProduct() override = default;
	TypedProduct(const TypedProduct&) = delete;
	TypedProduct& operator=(const TypedProduct&) = delete;
	TypedProduct(TypedProduct&&) = delete;
	TypedProduct& operator=(TypedProduct&&) = delete;

	[[nodiscard]] virtual Type type() const = 0;
	[[nodiscard]] virtual Buffer data() const = 0;

	[[nodiscard]] Buffer serialize() const final;
};
