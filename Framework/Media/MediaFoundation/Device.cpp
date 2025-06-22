#include "Media/MediaFoundation/Device.hpp"

#include "Com/TaskMemoryReleaser.hpp"
#include "Utils/Strings.hpp"

MediaFoundation::Device::Device(const uint32_t index, IMFActivate* const device, const MediaType::Type source_type) :
	m_index(index),
	m_device(device),
	m_source_type(source_type)
{
}

MediaFoundation::MediaSource MediaFoundation::Device::activate()
{
	IMFMediaSource* media_source = nullptr;

	const HRESULT result = get()->ActivateObject(IID_PPV_ARGS(&media_source));
	if (FAILED(result))
	{
		throw ComException(ErrorCode::FAILED_MEDIA_FOUNDATION_ACTIVATE_DEVICE, result);
	}

	return MediaSource(media_source);
}

uint32_t MediaFoundation::Device::get_index() const
{
	return m_index;
}

std::wstring MediaFoundation::Device::get_friendly_name() const
{
	return get_allocated_string(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME);
}

std::wstring MediaFoundation::Device::get_symbolic_link() const
{
	return get_allocated_string(
		m_source_type == MediaType::Type::VIDEO
			? MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK
			: MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK
	);
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
		throw ComException(ErrorCode::FAILED_MEDIA_FOUNDATION_GET_ALLOCATED_STRING, result);
	}

	Com::TaskMemoryReleaser string_guard(string);

	return std::wstring{string, string + static_cast<SSIZE_T>(string_length)};
}

std::wstring MediaFoundation::Device::type() const
{
	return L"Media Device";
}

ISerializableStruct::Fields MediaFoundation::Device::fields() const
{
	return {
		{L"index", Strings::to_wstring(m_index)},
		{L"friendly_name", get_friendly_name()},
		{L"symbolic_link", get_symbolic_link()},
	};
}
