#include "Products/TextTypedProduct.hpp"

#include "Utils/Strings.hpp"

TextTypedProduct::TextTypedProduct(std::wstring text):
	m_text(std::move(text))
{
}

TypedProduct::Type TextTypedProduct::type() const
{
	return Type::TEXT;
}

Buffer TextTypedProduct::data() const
{
	return Strings::to_buffer(m_text);
}
