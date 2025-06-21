#include "Media/MediaFoundation/Attributes.hpp"

#include "Wmi/ComTaskMemoryReleaser.hpp"
#include "Wmi/WmiException.hpp"

#include <mfapi.h>

MediaFoundation::Attributes::Attributes():
	m_attributes(create_attributes())
{
}

void MediaFoundation::Attributes::set_source_type(const SourceType source_type)
{
	GUID selected_guid;

	switch (source_type)
	{
	case SourceType::VIDEO:
	{
		selected_guid = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID;
		break;
	}
	case SourceType::AUDIO:
	{
		selected_guid = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID;
		break;
	}
	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}

	static constexpr GUID SOURCE_TYPE_GUID = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE;

	const HRESULT result = get()->SetGUID(SOURCE_TYPE_GUID, selected_guid);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_SET_SOURCE_TYPE, result);
	}
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

std::vector<MediaFoundation::Device> MediaFoundation::Attributes::enumerate_devices()
{
	IMFActivate** devices = nullptr;
	UINT32 devices_count = 0;

	const HRESULT result = MFEnumDeviceSources(get(), &devices, &devices_count);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_ENUMERATE_DEVICES, result);
	}

	const ComTaskMemoryReleaser devices_memory_guard(devices);

	std::vector<Device> result_devices;

	for (UINT32 i = 0; i < devices_count; ++i)
	{
		Device device(devices[i]);
		result_devices.emplace_back(std::move(device));
	}

	return result_devices;
}
