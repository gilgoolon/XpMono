#include "EnumerateMediaDevicesHandler.hpp"

#include "SerializableSection.hpp"
#include "Media/MediaFoundation/Attributes.hpp"
#include "Media/MediaFoundation/Instance.hpp"
#include "Products/TextTypedProduct.hpp"

EnumerateMediaDevicesHandler::EnumerateMediaDevicesHandler(std::unique_ptr<Event> operation_event,
                                                           const MediaFoundation::Attributes::SourceType media_type):
	IOperationHandler(std::move(operation_event)),
	m_media_type(media_type)
{
}

void EnumerateMediaDevicesHandler::run()
{
	MediaFoundation::Instance instance;

	MediaFoundation::Attributes attributes;
	attributes.set_source_type(m_media_type);

	SerializableSection devices_section{
		.name = std::wstring(
			m_media_type == MediaFoundation::Attributes::SourceType::VIDEO ? L"Video" : L"Audio"
		) + L" Devices",
		.objects = {}
	};

	for (std::unique_ptr<MediaFoundation::Device>& device : attributes.enumerate_devices())
	{
		devices_section.objects.emplace_back(std::move(device));
	}

	append(std::make_unique<TextTypedProduct>(devices_section.serialize()));
	finished();
}
