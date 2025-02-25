#include "Utils/Ip.hpp"

#include "Exception.hpp"
#include "Utils/Strings.hpp"

uint32_t Ip::parse_ip(const std::string& ip)
{
	static constexpr char IP_BYTE_SEPARATOR = '.';
	const std::vector<std::string> tokens = Strings::split(ip, IP_BYTE_SEPARATOR);
	if (tokens.size() != sizeof(uint32_t))
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}

	uint32_t result = 0;
	for (const std::string& token : tokens)
	{
		static constexpr uint32_t ONE_BYTE = 8;
		result <<= ONE_BYTE;

		const uint8_t byte = Strings::parse_uint8(token);
		result |= byte;
	}
	return result;
}
