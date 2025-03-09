#include "RawProduct.hpp"

RawProduct::RawProduct(const ICommand::Ptr& command, Buffer data):
	IProduct(command),
	m_data(std::move(data))
{
}

IProduct::Type RawProduct::type() const
{
	return Type::RAW_PRODUCT;
}

Buffer RawProduct::data() const
{
	return m_data;
}
