#include "Media/MediaFoundation/Device.hpp"

#include "Wmi/ComTaskMemoryReleaser.hpp"
#include "Wmi/WmiException.hpp"

MediaFoundation::Device::Device(IMFActivate* const device):
	m_device(device)
{
}

MediaFoundation::MediaSource MediaFoundation::Device::activate()
{
	IMFMediaSource* media_source = nullptr;

	const HRESULT result = get()->ActivateObject(IID_PPV_ARGS(&media_source));
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_ACTIVATE_DEVICE, result);
	}

	return MediaSource(media_source);
}

std::wstring MediaFoundation::Device::get_friendly_name() const
{
	return get_allocated_string(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME);
}

std::wstring MediaFoundation::Device::get_symbolic_link() const
{
	return get_allocated_string(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK);
}

IMFActivate* MediaFoundation::Device::get() const
{
	return reinterpret_cast<IMFActivate*>(m_device.get());
}

std::wstring MediaFoundation::Device::get_allocated_string(const GUID& property_guid) const
{
	WCHAR* string = nullptr;
	UINT32 string_length = 0;

	const HRESULT result = get()->GetAllocatedString(property_guid, &string, &string_length);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_GET_ALLOCATED_STRING, result);
	}

	ComTaskMemoryReleaser string_guard(string);

	return std::wstring{string, string + static_cast<SSIZE_T>(string_length)};
}
