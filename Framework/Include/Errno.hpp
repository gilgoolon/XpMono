#pragma once
#include <cerrno>

namespace Errno
{
[[nodiscard]] bool is_error(errno_t code);
};
