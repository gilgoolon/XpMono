#include "Errno.hpp"

bool Errno::is_error(const errno_t code)
{
	static constexpr errno_t ERRNO_SUCCESS = 0;
	return code != ERRNO_SUCCESS;
}
