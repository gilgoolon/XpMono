#include "Utils/Time.hpp"

#include "Errno.hpp"
#include "Exception.hpp"

#include <iomanip>
#include <sstream>

void Time::sleep(const Duration duration)
{
	Sleep(duration_cast<Millis>(duration).count());
}

Time::Datetime Time::now()
{
	return std::chrono::system_clock::now();
}

Time::Datetime Time::to_datetime(const int64_t file_time)
{
	static constexpr uint32_t BITS_IN_BYTE = 8;
	FILETIME ft{};
	ft.dwHighDateTime = static_cast<uint32_t>(file_time >> (sizeof(uint32_t) * BITS_IN_BYTE));
	ft.dwLowDateTime = static_cast<uint32_t>(file_time);
	return to_datetime(ft);
}

Time::Datetime Time::to_datetime(const FILETIME& ft)
{
	ULARGE_INTEGER ull{};
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;

	static constexpr int64_t FILETIME_EPOCH_DIFF = 116444736000000000LL;

	int64_t total_100ns = static_cast<int64_t>(ull.QuadPart);
	int64_t unix_time_100ns = total_100ns - FILETIME_EPOCH_DIFF;

	auto duration_since_epoch = duration_cast<std::chrono::system_clock::duration>(
		Nanos(unix_time_100ns * 100)
	);
	auto result = Datetime(duration_since_epoch);
	return result;
}

Time::Datetime Time::to_datetime(const SYSTEMTIME& st)
{
	FILETIME result{};
	if (SystemTimeToFileTime(&st, &result) == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_SYSTEM_TIME_CONVERSION);
	}
	return to_datetime(result);
}

Time::Datetime Time::from_webkit_time(const int64_t webkit_time)
{
	static constexpr int64_t MICRO_TO_100_NANO_INTERVALS = 10;
	const int64_t file_time_epoch = webkit_time * MICRO_TO_100_NANO_INTERVALS;

	return to_datetime(file_time_epoch);
}

std::string Time::to_string(const Datetime& dt)
{
	const time_t time_t = std::chrono::system_clock::to_time_t(dt);
	std::tm tm = {};
	if (Errno::is_error(localtime_s(&tm, &time_t)))
	{
		throw Exception(ErrorCode::FAILED_TIME_FORMAT_CONVERSION);
	}

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}

std::string Time::to_string(const Duration& duration)
{
	std::ostringstream output;
	Duration left = duration;
	if (const auto days = duration_cast<Days>(left); days.count() > 0)
	{
		output << days.count() << " days ";
		left -= days;
	}
	if (const auto hours = duration_cast<Hours>(left); hours.count() > 0)
	{
		output << hours.count() << " hrs ";
		left -= hours;
	}
	if (const auto minutes = duration_cast<Minutes>(left); minutes.count() > 0)
	{
		left -= minutes;
		output << minutes.count() << " mins ";
	}
	if (const auto seconds = duration_cast<Seconds>(left); seconds.count() > 0)
	{
		left -= seconds;
		output << seconds.count() << " secs ";
	}
	const std::string result = output.str();
	return result.empty() ? "< 1 sec" : result.substr(0, result.size() - 1);
}
