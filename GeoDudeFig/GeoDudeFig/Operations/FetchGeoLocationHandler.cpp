#include <External/cpp-httplib/httplib.h>  // messy includes - don't keep as first include

#include "FetchGeoLocationHandler.hpp"

#include "DiscoverNetworksHandler.hpp"
#include "SerializableSection.hpp"
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

FetchGeoLocationHandler::Location::Location(const double latitude, const double longitude, const double accuracy):
	latitude(latitude),
	longitude(longitude),
	accuracy(accuracy)
{
}

std::wstring FetchGeoLocationHandler::Location::type() const
{
	return L"Location";
}

ISerializableStruct::Fields FetchGeoLocationHandler::Location::fields() const
{
	return {
		{L"latitude", Strings::precise_to_wstring(latitude)},
		{L"longitude", Strings::precise_to_wstring(longitude)},
		{L"accuracy", Strings::to_wstring(accuracy)},
	};
}

void FetchGeoLocationHandler::run()
{
	std::vector<std::unique_ptr<Wireless::ReducedNetwork>> networks;

	for (const auto& network : Wireless::enumerate_networks())
	{
		std::vector<std::unique_ptr<Wireless::ReducedNetwork>> expanded = expand(network);
		networks.insert(
			networks.end(),
			std::make_move_iterator(expanded.begin()),
			std::make_move_iterator(expanded.end())
		);
	}

	SerializableSection geolocation_section{
		.name = L"Geolocation",
		.objects = {}
	};

	geolocation_section.objects.push_back(call_api_get_geolocation(networks));

	append(std::make_unique<TextTypedProduct>(geolocation_section.serialize()));
	finished();
}

Json FetchGeoLocationHandler::create_api_request_parameters(
	const std::vector<std::unique_ptr<Wireless::ReducedNetwork>>& networks)
{
	Json structured_networks = Json::array();
	for (const std::unique_ptr<Wireless::ReducedNetwork>& network : networks)
	{
		structured_networks.push_back(
			{
				{"macAddress", Strings::to_string(Wireless::format_mac_address(network->station.bssid))},
				{"age", 0},
				{"signalStrength", network->station.signal_strength_dbm}
			}
		);
	}

	Json parameters = {
		{"considerIp", "true"},
		{"wifiAccessPoints", structured_networks}
	};

	return parameters;
}

std::unique_ptr<FetchGeoLocationHandler::Location> FetchGeoLocationHandler::call_api_get_geolocation(
	const std::vector<std::unique_ptr<Wireless::ReducedNetwork>>& networks)
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

	return std::make_unique<Location>(latitude, longitude, accuracy);
}
