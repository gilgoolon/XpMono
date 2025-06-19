#include <External/cpp-httplib/httplib.h>  // messy includes - don't keep as first include

#include "FetchGeoLocationHandler.hpp"

#include "DiscoverNetworksHandler.hpp"
#include "Trace.hpp"
#include "Networking/Wireless.hpp"
#include "Products/TextTypedProduct.hpp"
#include "Utils/Formatting.hpp"

namespace Parameters
{
static constexpr auto API_KEY = "api_key";
}

FetchGeoLocationHandler::FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, std::string api_key):
	IOperationHandler(std::move(operation_event)),
	m_api_key(std::move(api_key)),
	m_http_client(new httplib::Client(GOOGLE_GEO_LOCATION_API_URL))
{
}

FetchGeoLocationHandler::FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, const Json& parameters):
	FetchGeoLocationHandler(std::move(operation_event), parameters[Parameters::API_KEY].get<std::string>())
{
}

FetchGeoLocationHandler::FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, const Buffer& raw_parameters):
	FetchGeoLocationHandler(std::move(operation_event), Json::parse(Strings::to_string(raw_parameters)))
{
}

FetchGeoLocationHandler::~FetchGeoLocationHandler()
{
	try
	{
		delete m_http_client;
	}
	catch (...)
	{
		TRACE(L"failed to destroy httplib::Client");
	}
}

std::wstring FetchGeoLocationHandler::Location::serialize() const
{
	const Formatting::Fields fields = {
		{L"latitude", Strings::precise_to_wstring(latitude)},
		{L"longitude", Strings::precise_to_wstring(longitude)},
		{L"accuracy", Strings::to_wstring(accuracy)},
	};

	return Formatting::format_fields(fields);
}

std::wstring FetchGeoLocationHandler::format_location(const Location& location)
{
	return Strings::concat(
		std::wstring{L"#Location"},
		std::wstring{L"\n"},
		location.serialize()
	);
}

void FetchGeoLocationHandler::run()
{
	std::vector<Wireless::ReducedNetwork> networks;

	for (const auto& network : Wireless::enumerate_networks())
	{
		const std::vector<Wireless::ReducedNetwork> expanded = expand(network);
		networks.insert(networks.end(), expanded.begin(), expanded.end());
	}

	std::wstring product;
	product.append(L"[Geolocation]\n");

	const Location result = call_api_get_geolocation(networks);
	product.append(format_location(result));

	append(std::make_unique<TextTypedProduct>(product));
	finished();
}

Json FetchGeoLocationHandler::create_api_request_parameters(const std::vector<Wireless::ReducedNetwork>& networks)
{
	Json structured_networks = Json::array();
	for (const Wireless::ReducedNetwork& network : networks)
	{
		structured_networks.push_back(
			{
				{"macAddress", Strings::to_string(Wireless::format_mac_address(network.station.bssid))},
				{"age", 0},
				{"signalStrength", network.station.signal_strength_dbm}
			}
		);
	}

	Json parameters = {
		{"considerIp", "false"},
		{"wifiAccessPoints", structured_networks}
	};

	return parameters;
}

FetchGeoLocationHandler::Location FetchGeoLocationHandler::call_api_get_geolocation(
	const std::vector<Wireless::ReducedNetwork>& networks)
{
	static constexpr auto JSON_CONTENT_ENCODING_TYPE = "application/json";

	const std::string endpoint = std::string{GOOGLE_GEO_LOCATION_API_ENDPOINT} + m_api_key;
	const Json request_parameters = create_api_request_parameters(networks);
	const std::string request_data = request_parameters.dump();

	const httplib::Result result = m_http_client->Post(endpoint, request_data, JSON_CONTENT_ENCODING_TYPE);

	static constexpr int EXPECTED_STATUS_CODE_OK = 200;
	if (result.error() != httplib::Error::Success ||
		result->status != EXPECTED_STATUS_CODE_OK)
	{
		throw Exception(ErrorCode::FAILED_HTTP_POST);
	}

	Json result_data = Json::parse(result->body);
	const Json location_data = result_data["location"];
	const double latitude = location_data["lat"];
	const double longitude = location_data["lng"];
	const double accuracy = result_data["accuracy"];

	return {.latitude = latitude, .longitude = longitude, .accuracy = accuracy};
}
