#include "Communicators/Protocol/IRequest.hpp"

#include "Utils/Strings.hpp"

IRequest::IRequest(const Type type, const uint32_t liver_id):
	m_type(type),
	m_liver_id(liver_id)
{
}

Buffer IRequest::serialize() const
{
	return Strings::concat(ISerializable::serialize(m_type), ISerializable::serialize(m_liver_id), data());
}
