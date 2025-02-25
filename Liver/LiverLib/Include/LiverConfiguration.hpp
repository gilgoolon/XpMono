#pragma once
#include "Utils/Buffer.hpp"
#include "Utils/Time.hpp"

#include <cstdint>

struct LiverConfiguration final
{
	uint32_t cnc_ip;
	uint16_t cnc_port;
	Time::Duration iteration_delay;

	[[nodiscard]] static LiverConfiguration parse(const Buffer& data);
};
