#pragma once
#include "IOperationHandler.hpp"

class SystemInformationHandler final : public IOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = 1;

	explicit SystemInformationHandler(std::unique_ptr<Event> operation_event);
	~SystemInformationHandler() override = default;
	SystemInformationHandler(const SystemInformationHandler&) = delete;
	SystemInformationHandler& operator=(const SystemInformationHandler&) = delete;
	SystemInformationHandler(SystemInformationHandler&&) = delete;
	SystemInformationHandler& operator=(SystemInformationHandler&&) = delete;

	void run() override;
};
