#include "Media/MediaFoundation/MediaType.hpp"

#include "Wmi/WmiException.hpp"

#include <mfapi.h>

MediaFoundation::MediaType::MediaType(IMFMediaType* media_type):
	m_type(media_type)
{
}

MediaFoundation::MediaType::MediaType(const Type type, const SubType sub_type):
	MediaType(create_media_type())
{
	set_guid(MF_MT_MAJOR_TYPE, get_type_guid(type));
	set_guid(MF_MT_SUBTYPE, get_sub_type_guid(sub_type));
}

MediaFoundation::MediaType::Dimensions MediaFoundation::MediaType::get_dimensions() const
{
	UINT32 width = 0;
	UINT32 height = 0;

	const HRESULT result = MFGetAttributeSize(get(), MF_MT_FRAME_SIZE, &width, &height);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_GET_DIMENSIONS, result);
	}

	return Dimensions{.width = width, .height = height};
}

IMFMediaType* MediaFoundation::MediaType::get() const
{
	return reinterpret_cast<IMFMediaType*>(m_type.get());
}

void MediaFoundation::MediaType::set_guid(const GUID& key, const GUID& value)
{
	const HRESULT result = get()->SetGUID(key, value);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_MEDIA_TYPE_SET_GUID, result);
	}
}

IMFMediaType* MediaFoundation::MediaType::create_media_type()
{
	IMFMediaType* type = nullptr;

	const HRESULT result = MFCreateMediaType(&type);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_CREATE_MEDIA_TYPE, result);
	}

	return type;
}

GUID MediaFoundation::MediaType::get_type_guid(const Type type)
{
	GUID guid;

	switch (type)
	{
	case Type::VIDEO:
	{
		guid = MFMediaType_Video;
		break;
	}
	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}

	return guid;
}

GUID MediaFoundation::MediaType::get_sub_type_guid(const SubType sub_type)
{
	GUID guid;

	switch (sub_type)
	{
	case SubType::VIDEO_FORMAT_RGB32:
	{
		guid = MFVideoFormat_RGB32;
		break;
	}
	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}

	return guid;
}
