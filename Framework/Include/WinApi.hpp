#pragma once
#include "WaitStatus.hpp"
#include "Utils/Time.hpp"

namespace WinApi
{
[[nodiscard]] WaitStatus wait(HANDLE handle, Time::Duration timeout);

void sleep(Time::Duration duration);
}
