#pragma once
#include "Handlers/IOperationHandler.hpp"

class StartSniffHandler final : public IOperationHandler
{
public:
	using IOperationHandler::IOperationHandler;
	~StartSniffHandler() override = default;
	StartSniffHandler(const StartSniffHandler&) = delete;
	StartSniffHandler& operator=(const StartSniffHandler&) = delete;
	StartSniffHandler(StartSniffHandler&&) = delete;
	StartSniffHandler& operator=(StartSniffHandler&&) = delete;

	void run() override;
};
