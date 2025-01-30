#pragma once
#include <cstdint>

namespace WinApi
{
enum class WaitStatus : uint32_t
{
	FINISHED = 0,
	OBJECT_CLOSED,
	TIMEOUT,
	FAILED,
};
}
