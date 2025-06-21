#include "EnumerateMediaDevicesHandler.hpp"

EnumerateMediaDevicesHandler::EnumerateMediaDevicesHandler(std::unique_ptr<Event> operation_event):
	IOperationHandler(std::move(operation_event))
{
}

void EnumerateMediaDevicesHandler::run()
{
}
