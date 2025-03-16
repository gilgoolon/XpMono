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

	explicit TypedProduct();
	~TypedProduct() override;
	TypedProduct(const TypedProduct&) = delete;
	TypedProduct& operator=(const TypedProduct&) = delete;
	TypedProduct(TypedProduct&&) = delete;
	TypedProduct& operator=(TypedProduct&&) = delete;

	[[nodiscard]] virtual Type type() const = 0;
	[[nodiscard]] virtual Type data() const = 0;

	[[nodiscard]] Buffer serialize() const override;
};
