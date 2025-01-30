#include "Utils/Time.hpp"

#include "Errno.hpp"
#include "Exception.hpp"

#include <iomanip>
#include <sstream>

Time::Datetime Time::now()
{
	return std::chrono::system_clock::now();
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
