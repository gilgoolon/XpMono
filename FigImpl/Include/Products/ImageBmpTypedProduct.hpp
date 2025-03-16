#pragma once
#include "RawTypedProduct.hpp"

class ImageBmpTypedProduct final : public RawTypedProduct
{
public:
	using RawTypedProduct::RawTypedProduct;
	~ImageBmpTypedProduct() override = default;
	ImageBmpTypedProduct(const ImageBmpTypedProduct&) = delete;
	ImageBmpTypedProduct& operator=(const ImageBmpTypedProduct&) = delete;
	ImageBmpTypedProduct(ImageBmpTypedProduct&&) = delete;
	ImageBmpTypedProduct& operator=(ImageBmpTypedProduct&&) = delete;

	[[nodiscard]] Type type() const override;
};
