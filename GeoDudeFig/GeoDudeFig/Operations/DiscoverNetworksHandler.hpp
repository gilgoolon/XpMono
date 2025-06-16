#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"

class DiscoverNetworksHandler final : public IOperationHandler
{
public:
	explicit DiscoverNetworksHandler(std::unique_ptr<Event> operation_event);
	~DiscoverNetworksHandler() override = default;
	DiscoverNetworksHandler(const DiscoverNetworksHandler&) = delete;
	DiscoverNetworksHandler& operator=(const DiscoverNetworksHandler&) = delete;
	DiscoverNetworksHandler(DiscoverNetworksHandler&&) = delete;
	DiscoverNetworksHandler& operator=(DiscoverNetworksHandler&&) = delete;

private:
	[[nodiscard]] static std::wstring format_network(const Wireless::Network& network, size_t index);

public:
	void run() override;
};
