#include "EnumerateMediaDevicesHandler.hpp"

#include "Media/MediaFoundation/Attributes.hpp"
#include "Media/MediaFoundation/Instance.hpp"

EnumerateMediaDevicesHandler::EnumerateMediaDevicesHandler(std::unique_ptr<Event> operation_event):
	IOperationHandler(std::move(operation_event))
{
}

void EnumerateMediaDevicesHandler::run()
{
	MediaFoundation::Instance instance;

	MediaFoundation::Attributes attributes;
	attributes.enumerate_devices();
}
