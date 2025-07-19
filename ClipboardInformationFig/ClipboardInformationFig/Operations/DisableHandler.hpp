#pragma once
#include "Handlers/IOperationHandler.hpp"

class DisableHandler final : public IOperationHandler
{
public:
	using IOperationHandler::IOperationHandler;
	~DisableHandler() override = default;
	DisableHandler(const DisableHandler&) = delete;
	DisableHandler& operator=(const DisableHandler&) = delete;
	DisableHandler(DisableHandler&&) = delete;
	DisableHandler& operator=(DisableHandler&&) = delete;

	void run() override;
};
