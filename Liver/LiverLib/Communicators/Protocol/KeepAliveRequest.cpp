#include "Communicators/Protocol/KeepAliveRequest.hpp"

#include "Utils/Strings.hpp"

KeepAliveRequest::KeepAliveRequest(const uint32_t liver_id):
	IRequest(Type::KEEP_ALIVE, liver_id)
{
}

Buffer KeepAliveRequest::data() const
{
	return {};
}
