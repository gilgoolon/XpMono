#pragma once
#include "IOperationHandler.hpp"

class PhysicalDrivesInformationHandler final : public IOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = 2;

	using IOperationHandler::IOperationHandler;
	~PhysicalDrivesInformationHandler() override = default;
	PhysicalDrivesInformationHandler(const PhysicalDrivesInformationHandler&) = delete;
	PhysicalDrivesInformationHandler& operator=(const PhysicalDrivesInformationHandler&) = delete;
	PhysicalDrivesInformationHandler(PhysicalDrivesInformationHandler&&) = delete;
	PhysicalDrivesInformationHandler& operator=(PhysicalDrivesInformationHandler&&) = delete;

	void run() override;
};
