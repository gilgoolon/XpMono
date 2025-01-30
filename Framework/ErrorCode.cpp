#include "ErrorCode.hpp"

std::wostringstream& operator<<(std::wostringstream& output, ErrorCode code)
{
	output << static_cast<uint32_t>(code);
	return output;
}
