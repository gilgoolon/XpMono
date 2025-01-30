#pragma once

#include <chrono>
#include <string>
#include <windows.h>

namespace Time
{
using Rep = uint32_t;
using Datetime = std::chrono::system_clock::time_point;
using Nanos = std::chrono::duration<uint64_t, std::chrono::nanoseconds::period>;
using Millis = std::chrono::duration<Rep, std::chrono::milliseconds::period>;
using Seconds = std::chrono::duration<Rep, std::chrono::seconds::period>;
using Minutes = std::chrono::duration<Rep, std::chrono::minutes::period>;
using Hours = std::chrono::duration<Rep, std::chrono::hours::period>;
using Days = std::chrono::duration<Rep, std::ratio<std::chrono::hours::period::num * 24>>;
using Duration = Millis;
using SystemDuration = Nanos;

using std::chrono::duration_cast;

static constexpr Duration INSTANT = Duration::zero();

[[nodiscard]] Datetime now();

[[nodiscard]] Datetime to_datetime(const FILETIME& ft);

[[nodiscard]] std::string to_string(const Datetime& dt);

[[nodiscard]] std::string to_string(const Duration& duration);
}
