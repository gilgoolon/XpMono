#include "Communicators/Protocol/SendRandomResponse.hpp"

SendRandomResponse::SendRandomResponse(const uint32_t value):
	m_value(value)
{
}

IResponse::Type SendRandomResponse::type() const
{
	return Type::SEND_RANDOM;
}

uint32_t SendRandomResponse::value() const
{
	return m_value;
}
