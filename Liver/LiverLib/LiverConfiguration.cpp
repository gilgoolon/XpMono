#include "LiverConfiguration.hpp"

#include "Json.hpp"
#include "Networking/Socket.hpp"
#include "Utils/Ip.hpp"
#include "Utils/Strings.hpp"

namespace Params
{
static constexpr std::string_view CNC_IP = "cnc_ip";
static constexpr std::string_view CNC_PORT = "cnc_port";
static constexpr std::string_view ITERATION_DELAY = "iteration_delay";
}

LiverConfiguration LiverConfiguration::parse(const Buffer& data)
{
	const Json config = Strings::to_string(data);

	const uint32_t cnc_ip = Ip::parse_ip(config.at(std::string{Params::CNC_IP}).get<std::string>());
	const uint16_t cnc_port = config.at(std::string{Params::CNC_PORT}).get<uint16_t>();
	const uint32_t iteration_delay_seconds = config.at(std::string{Params::ITERATION_DELAY}).get<uint32_t>();
	const Time::Duration iteration_delay = Time::Seconds(iteration_delay_seconds);

	return {cnc_ip, cnc_port, iteration_delay};
}
