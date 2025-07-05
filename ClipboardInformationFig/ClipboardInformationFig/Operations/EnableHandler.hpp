#pragma once
#include "Handlers/IOperationHandler.hpp"

class EnableHandler final : public IOperationHandler
{
public:
	using IOperationHandler::IOperationHandler;
	~EnableHandler() override = default;
	EnableHandler(const EnableHandler&) = delete;
	EnableHandler& operator=(const EnableHandler&) = delete;
	EnableHandler(EnableHandler&&) = delete;
	EnableHandler& operator=(EnableHandler&&) = delete;

	void run() override;
};
