#include "Products/ErrorProduct.hpp"

ErrorProduct::ErrorProduct(const ICommand::Ptr& command, const ErrorCode code) :
	IProduct(command),
	m_code(code)
{
}

IProduct::Type ErrorProduct::type() const
{
	return Type::ERROR_PRODUCT;
}

Buffer ErrorProduct::data() const
{
	return ISerializable::serialize(m_code);
}
