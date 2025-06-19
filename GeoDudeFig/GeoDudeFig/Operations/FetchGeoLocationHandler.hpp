#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"
#include "Synchronization/Event.hpp"

#include <memory>

class FetchGeoLocationHandler final : public IOperationHandler
{
public:
	explicit FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, std::string api_key);
	explicit FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, const Json& parameters);
	explicit FetchGeoLocationHandler(std::unique_ptr<Event> operation_event, const Buffer& raw_parameters);
	~FetchGeoLocationHandler() override = default;
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
};
