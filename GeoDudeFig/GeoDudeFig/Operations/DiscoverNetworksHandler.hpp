#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"
#include "Networking/WlanClient.hpp"

class DiscoverNetworksHandler final : public IOperationHandler
{
public:
	explicit DiscoverNetworksHandler(std::unique_ptr<Event> operation_event, bool reduce);
	explicit DiscoverNetworksHandler(std::unique_ptr<Event> operation_event, const Json& parameters);
	explicit DiscoverNetworksHandler(std::unique_ptr<Event> operation_event, const Buffer& raw_parameters);
	~DiscoverNetworksHandler() override = default;
	DiscoverNetworksHandler(const DiscoverNetworksHandler&) = delete;
	DiscoverNetworksHandler& operator=(const DiscoverNetworksHandler&) = delete;
	DiscoverNetworksHandler(DiscoverNetworksHandler&&) = delete;
	DiscoverNetworksHandler& operator=(DiscoverNetworksHandler&&) = delete;

private:
	[[nodiscard]] static std::wstring format_network(const Wireless::ReducedNetwork& network);

public:
	void run() override;

private:
	bool m_reduce;
};
