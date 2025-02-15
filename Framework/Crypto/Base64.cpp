#include "Crypto/Base64.hpp"

#include "External/base64/base64.hpp"
#include "Utils/Strings.hpp"

#include <string>

Buffer Base64::decode(const std::string& str64)
{
	return Strings::to_buffer(base64::from_base64(str64));
}

std::string Base64::encode(const Buffer& data)
{
	return base64::to_base64({reinterpret_cast<const char*>(data.data()), data.size()});
}
