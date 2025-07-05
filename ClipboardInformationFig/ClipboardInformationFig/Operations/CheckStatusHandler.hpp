#pragma once
#include "Handlers/IOperationHandler.hpp"

class CheckStatusHandler final : public IOperationHandler
{
public:
	using IOperationHandler::IOperationHandler;
	~CheckStatusHandler() override = default;
	CheckStatusHandler(const CheckStatusHandler&) = delete;
	CheckStatusHandler& operator=(const CheckStatusHandler&) = delete;
	CheckStatusHandler(CheckStatusHandler&&) = delete;
	CheckStatusHandler& operator=(CheckStatusHandler&&) = delete;

	void run() override;
};
