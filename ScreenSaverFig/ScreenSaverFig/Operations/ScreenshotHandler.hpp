#pragma once
#include "Handlers/IOperationHandler.hpp"

class ScreenshotHandler final : public IOperationHandler
{
public:
	using IOperationHandler::IOperationHandler;
	~ScreenshotHandler() override = default;
	ScreenshotHandler(const ScreenshotHandler&) = delete;
	ScreenshotHandler& operator=(const ScreenshotHandler&) = delete;
	ScreenshotHandler(ScreenshotHandler&&) = delete;
	ScreenshotHandler& operator=(ScreenshotHandler&&) = delete;

	void run() override;
};
