#include "Communicators/Protocol/KeepAliveRequest.hpp"

Buffer KeepAliveRequest::serialize() const
{
	return ISerializable::serialize(Type::KEEP_ALIVE);
}
