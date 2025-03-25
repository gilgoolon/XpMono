#pragma once
#include "Handlers/IOperationHandler.hpp"

class StopSniffHandler final : public IOperationHandler
{
public:
	using IOperationHandler::IOperationHandler;
	~StopSniffHandler() override = default;
	StopSniffHandler(const StopSniffHandler&) = delete;
	StopSniffHandler& operator=(const StopSniffHandler&) = delete;
	StopSniffHandler(StopSniffHandler&&) = delete;
	StopSniffHandler& operator=(StopSniffHandler&&) = delete;

	void run() override;
};
