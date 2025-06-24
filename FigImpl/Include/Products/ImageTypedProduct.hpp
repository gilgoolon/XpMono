#pragma once
#include "RawTypedProduct.hpp"

class ImageTypedProduct final : public RawTypedProduct
{
public:
	using RawTypedProduct::RawTypedProduct;
	~ImageTypedProduct() override = default;
	ImageTypedProduct(const ImageTypedProduct&) = delete;
	ImageTypedProduct& operator=(const ImageTypedProduct&) = delete;
	ImageTypedProduct(ImageTypedProduct&&) = delete;
	ImageTypedProduct& operator=(ImageTypedProduct&&) = delete;

	[[nodiscard]] Type type() const override;
};
