#include "EnumerateMediaDevicesHandler.hpp"

#include "SerializableSection.hpp"
#include "Media/MediaFoundation/Attributes.hpp"
#include "Media/MediaFoundation/Instance.hpp"
#include "Products/TextTypedProduct.hpp"

EnumerateMediaDevicesHandler::EnumerateMediaDevicesHandler(std::unique_ptr<Event> operation_event):
	IOperationHandler(std::move(operation_event))
{
}

void EnumerateMediaDevicesHandler::run()
{
	MediaFoundation::Instance instance;

	MediaFoundation::Attributes attributes;
	attributes.set_source_type(MediaFoundation::Attributes::SourceType::VIDEO);

	SerializableSection devices_section{.name = L"Media Devices", .objects = {}};

	for (std::unique_ptr<MediaFoundation::Device>& device : attributes.enumerate_devices())
	{
		devices_section.objects.emplace_back(std::move(device));
	}

	append(std::make_unique<TextTypedProduct>(devices_section.serialize()));
	finished();
}
