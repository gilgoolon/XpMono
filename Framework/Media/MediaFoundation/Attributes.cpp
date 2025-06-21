#include "Media/MediaFoundation/Attributes.hpp"

#include "Wmi/ComTaskMemoryReleaser.hpp"
#include "Wmi/WmiException.hpp"

#include <mfapi.h>

MediaFoundation::Attributes::Attributes():
	m_attributes(create_attributes()),
	m_source_type()
{
}

void MediaFoundation::Attributes::set_source_type(const MediaType::Type source_type)
{
	GUID selected_guid;

	switch (source_type)
	{
	case MediaType::Type::VIDEO:
	{
		selected_guid = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID;
		break;
	}
	case MediaType::Type::AUDIO:
	{
		selected_guid = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID;
		break;
	}
	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}

	const GUID source_type_guid = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE;

	const HRESULT result = get()->SetGUID(source_type_guid, selected_guid);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_SET_SOURCE_TYPE, result);
	}

	m_source_type = source_type;
}

IMFAttributes* MediaFoundation::Attributes::get() const
{
	return reinterpret_cast<IMFAttributes*>(m_attributes.get());
}

IMFAttributes* MediaFoundation::Attributes::create_attributes()
{
	static constexpr DWORD SINGLE_OBJECT = 1;
	IMFAttributes* attributes = nullptr;

	const HRESULT result = MFCreateAttributes(&attributes, SINGLE_OBJECT);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_CREATE_ATTRIBUTES, result);
	}

	return attributes;
}

std::vector<std::unique_ptr<MediaFoundation::Device>> MediaFoundation::Attributes::enumerate_devices()
{
	IMFActivate** devices = nullptr;
	UINT32 devices_count = 0;

	const HRESULT result = MFEnumDeviceSources(get(), &devices, &devices_count);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_ENUMERATE_DEVICES, result);
	}

	const ComTaskMemoryReleaser devices_memory_guard(devices);

	std::vector<std::unique_ptr<Device>> result_devices;

	struct PublicDevice final : Device
	{
		explicit PublicDevice(const uint32_t index, IMFActivate* const device, MediaType::Type source_type):
			Device(index, device, source_type)
		{
		}
	};

	result_devices.reserve(devices_count);
	for (UINT32 i = 0; i < devices_count; ++i)
	{
		result_devices.emplace_back(std::make_unique<PublicDevice>(i, devices[i], m_source_type));
	}

	return result_devices;
}
