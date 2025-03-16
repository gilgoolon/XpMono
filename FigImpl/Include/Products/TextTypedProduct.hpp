#pragma once
#include "Products/TypedProduct.hpp"

#include <string>

class TextTypedProduct final : public TypedProduct
{
public:
	explicit TextTypedProduct(std::wstring text);
	~TextTypedProduct() override = default;
	TextTypedProduct(const TextTypedProduct&) = delete;
	TextTypedProduct& operator=(const TextTypedProduct&) = delete;
	TextTypedProduct(TextTypedProduct&&) = delete;
	TextTypedProduct& operator=(TextTypedProduct&&) = delete;

	[[nodiscard]] Type type() const override;
	[[nodiscard]] Buffer data() const override;

private:
	std::wstring m_text;
};
