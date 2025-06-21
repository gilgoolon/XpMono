#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"
#include "Synchronization/Event.hpp"

#include <memory>

class EnumerateMediaDevicesHandler final : public IOperationHandler
{
public:
	explicit EnumerateMediaDevicesHandler(std::unique_ptr<Event> operation_event);
	~EnumerateMediaDevicesHandler() override = default;
	EnumerateMediaDevicesHandler(const EnumerateMediaDevicesHandler&) = delete;
	EnumerateMediaDevicesHandler& operator=(const EnumerateMediaDevicesHandler&) = delete;
	EnumerateMediaDevicesHandler(EnumerateMediaDevicesHandler&&) = delete;
	EnumerateMediaDevicesHandler& operator=(EnumerateMediaDevicesHandler&&) = delete;

	void run() override;
};
