#pragma once
#include "Handlers/IOperationHandler.hpp"
#include "Handlers/WmiQueryMultiOperationHandler.hpp"
#include "../Api.hpp"

class PhysicalDrivesInformationHandler final : public WmiQueryMultiOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = static_cast<Fig::OperationType>(Api::OperationType::PHYSICAL_DRIVES_INFO);

	explicit PhysicalDrivesInformationHandler(std::unique_ptr<Event> operation_event);
	~PhysicalDrivesInformationHandler() override = default;
	PhysicalDrivesInformationHandler(const PhysicalDrivesInformationHandler&) = delete;
	PhysicalDrivesInformationHandler& operator=(const PhysicalDrivesInformationHandler&) = delete;
	PhysicalDrivesInformationHandler(PhysicalDrivesInformationHandler&&) = delete;
	PhysicalDrivesInformationHandler& operator=(PhysicalDrivesInformationHandler&&) = delete;
};
