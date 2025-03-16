#include "Products/RawTypedProduct.hpp"

RawTypedProduct::RawTypedProduct(Buffer data):
	m_data(std::move(data))
{
}

TypedProduct::Type RawTypedProduct::type() const
{
	return Type::RAW;
}

Buffer RawTypedProduct::data() const
{
	return m_data;
}
