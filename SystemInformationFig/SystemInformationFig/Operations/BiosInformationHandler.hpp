#pragma once
#include "Handlers/WmiQuerySingleOperationHandler.hpp"
#include "../Api.hpp"

class BiosInformationHandler final : public WmiQuerySingleOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = static_cast<Fig::OperationType>(Api::OperationType::BIOS_INFO);

	explicit BiosInformationHandler(std::unique_ptr<Event> operation_event);
	~BiosInformationHandler() override = default;
	BiosInformationHandler(const BiosInformationHandler&) = delete;
	BiosInformationHandler& operator=(const BiosInformationHandler&) = delete;
	BiosInformationHandler(BiosInformationHandler&&) = delete;
	BiosInformationHandler& operator=(BiosInformationHandler&&) = delete;
};
