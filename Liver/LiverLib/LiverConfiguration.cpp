#include "LiverConfiguration.hpp"

#include "Json.hpp"
#include "Networking/Socket.hpp"
#include "Utils/Ip.hpp"
#include "Utils/Strings.hpp"

namespace Params
{
	static constexpr auto CNC_IP = "cnc_ip";
	static constexpr auto CNC_PORT = "cnc_port";
	static constexpr auto ITERATION_DELAY = "iteration_delay";
}

LiverConfiguration LiverConfiguration::parse(const Buffer& data)
{
	const Json config = Json::parse(Strings::to_string(data));

	const uint32_t cnc_ip = Ip::parse_ip(config.at(Params::CNC_IP).get<std::string>());
	const uint16_t cnc_port = config.at(Params::CNC_PORT).get<uint16_t>();
	const uint32_t iteration_delay_seconds = config.at(Params::ITERATION_DELAY).get<uint32_t>();
	const Time::Duration iteration_delay = Time::Seconds(iteration_delay_seconds);

	return {cnc_ip, cnc_port, iteration_delay};
}
