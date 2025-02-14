#pragma once
#include "Handlers/WmiQuerySingleOperationHandler.hpp"

class BiosInformationHandler final : public WmiQuerySingleOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = 3;

	explicit BiosInformationHandler(std::unique_ptr<Event> operation_event);
	~BiosInformationHandler() override = default;
	BiosInformationHandler(const BiosInformationHandler&) = delete;
	BiosInformationHandler& operator=(const BiosInformationHandler&) = delete;
	BiosInformationHandler(BiosInformationHandler&&) = delete;
	BiosInformationHandler& operator=(BiosInformationHandler&&) = delete;
};
