#include "FetchGeoLocationHandler.hpp"

#include "Utils/Formatting.hpp"

namespace Parameters
{
static constexpr auto API_KEY = "api_key";
}

FetchGeoLocationHandler::FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, std::string api_key):
	IOperationHandler(std::move(operation_event)),
	m_api_key(std::move(api_key))

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

std::wstring FetchGeoLocationHandler::Location::serialize() const
{
	const Formatting::Fields fields = {
		{L"latitude", Strings::to_wstring(latitude)},
		{L"longitude", Strings::to_wstring(longitude)},
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
}
