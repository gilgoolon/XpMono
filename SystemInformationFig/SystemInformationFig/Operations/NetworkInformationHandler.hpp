#pragma once
#include "Handlers/WmiQueryMultiOperationHandler.hpp"
#include "../Api.hpp"
#include "Synchronization/Event.hpp"

class NetworkInformationHandler final : public WmiQueryMultiOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = static_cast<Fig::OperationType>(Api::OperationType::NETWORK_INFO);

	explicit NetworkInformationHandler(std::unique_ptr<Event> operation_event);
	~NetworkInformationHandler() override = default;
	NetworkInformationHandler(const NetworkInformationHandler&) = delete;
	NetworkInformationHandler& operator=(const NetworkInformationHandler&) = delete;
	NetworkInformationHandler(NetworkInformationHandler&&) = delete;
	NetworkInformationHandler& operator=(NetworkInformationHandler&&) = delete;
};
