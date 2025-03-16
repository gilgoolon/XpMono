#pragma once
#include "TypedProduct.hpp"

class RawTypedProduct : public TypedProduct
{
public:
	explicit RawTypedProduct(Buffer data);
	~RawTypedProduct() override = default;
	RawTypedProduct(const RawTypedProduct&) = delete;
	RawTypedProduct& operator=(const RawTypedProduct&) = delete;
	RawTypedProduct(RawTypedProduct&&) = delete;
	RawTypedProduct& operator=(RawTypedProduct&&) = delete;

	[[nodiscard]] Type type() const override;
	[[nodiscard]] Buffer data() const override;

private:
	Buffer m_data;
};
