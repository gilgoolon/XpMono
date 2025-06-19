#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"
#include "Networking/Wireless.hpp"
#include "Synchronization/Event.hpp"

namespace httplib
{
class Client;
}

class FetchGeoLocationHandler final : public IOperationHandler
{
public:
	explicit FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, std::string api_key);
	explicit FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, const Json& parameters);
	explicit FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, const Buffer& raw_parameters);
	~FetchGeoLocationHandler() override;
	FetchGeoLocationHandler(const FetchGeoLocationHandler&) = delete;
	FetchGeoLocationHandler& operator=(const FetchGeoLocationHandler&) = delete;
	FetchGeoLocationHandler(FetchGeoLocationHandler&&) = delete;
	FetchGeoLocationHandler& operator=(FetchGeoLocationHandler&&) = delete;

private:
	struct Location final
	{
		double latitude;
		double longitude;
		double accuracy;

		[[nodiscard]] std::wstring serialize() const;
	};

	[[nodiscard]] static std::wstring format_location(const Location& location);

public:
	void run() override;

private:
	std::string m_api_key;
	httplib::Client* m_http_client;

	static constexpr auto GOOGLE_GEO_LOCATION_API_URL = "https://www.googleapis.com";
	static constexpr auto GOOGLE_GEO_LOCATION_API_ENDPOINT = "/geolocation/v1/geolocate?key=";
	[[nodiscard]] static Json create_api_request_parameters(const std::vector<Wireless::ReducedNetwork>& networks);

	[[nodiscard]] Location call_api_get_geolocation(const std::vector<Wireless::ReducedNetwork>& networks);
};
