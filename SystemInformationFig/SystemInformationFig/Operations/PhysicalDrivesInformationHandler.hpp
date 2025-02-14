#pragma once
#include "Handlers/IOperationHandler.hpp"
#include "Handlers/WmiQuerySingleOperationHandler.hpp"

class PhysicalDrivesInformationHandler final : public WmiQuerySingleOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = 2;

	explicit PhysicalDrivesInformationHandler(std::unique_ptr<Event> operation_event);
	~PhysicalDrivesInformationHandler() override = default;
	PhysicalDrivesInformationHandler(const PhysicalDrivesInformationHandler&) = delete;
	PhysicalDrivesInformationHandler& operator=(const PhysicalDrivesInformationHandler&) = delete;
	PhysicalDrivesInformationHandler(PhysicalDrivesInformationHandler&&) = delete;
	PhysicalDrivesInformationHandler& operator=(PhysicalDrivesInformationHandler&&) = delete;
};
