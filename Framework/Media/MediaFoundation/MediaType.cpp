#include "Media/MediaFoundation/MediaType.hpp"

#include "Macros.hpp"
#include "Wmi/WmiException.hpp"

#include <array>
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

uint64_t MediaFoundation::MediaType::Dimensions::area() const
{
	return static_cast<uint64_t>(width) * height;
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

MediaFoundation::MediaType::SubType MediaFoundation::MediaType::get_sub_type() const
{
	return get_sub_type_enum(get_guid(MF_MT_SUBTYPE));
}

IMFMediaType* MediaFoundation::MediaType::get() const
{
	return reinterpret_cast<IMFMediaType*>(m_type.get());
}

GUID MediaFoundation::MediaType::get_guid(const GUID& key) const
{
	GUID value{};

	const HRESULT result = get()->GetGUID(key, &value);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_GET_GUID, result);
	}

	return value;
}

void MediaFoundation::MediaType::set_guid(const GUID& key, const GUID& value)
{
	const HRESULT result = get()->SetGUID(key, value);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_SET_GUID, result);
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
	case Type::AUDIO:
	{
		guid = MFMediaType_Audio;
		break;
	}
	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}

	return guid;
}

GUID MediaFoundation::MediaType::get_sub_type_guid(const SubType sub_type)
{
	return sub_type_map().at_key(sub_type);
}

MediaFoundation::MediaType::SubType MediaFoundation::MediaType::get_sub_type_enum(const GUID& sub_type)
{
	return sub_type_map().at_value(sub_type);
}

const BiMap<MediaFoundation::MediaType::SubType, GUID>& MediaFoundation::MediaType::sub_type_map()
{
	static const BiMap<SubType, GUID> MAP = {
		{SubType::VIDEO_FORMAT_RGB32, MFVideoFormat_RGB32},
		{SubType::VIDEO_FORMAT_ARGB32, MFVideoFormat_ARGB32},
		{SubType::VIDEO_FORMAT_RGB24, MFVideoFormat_RGB24},
		{SubType::VIDEO_FORMAT_RGB555, MFVideoFormat_RGB555},
		{SubType::VIDEO_FORMAT_RGB565, MFVideoFormat_RGB565},
		{SubType::VIDEO_FORMAT_RGB8, MFVideoFormat_RGB8},
		{SubType::VIDEO_FORMAT_L8, MFVideoFormat_L8},
		{SubType::VIDEO_FORMAT_L16, MFVideoFormat_L16},
		{SubType::VIDEO_FORMAT_D16, MFVideoFormat_D16},
		{SubType::VIDEO_FORMAT_AI44, MFVideoFormat_AI44},
		{SubType::VIDEO_FORMAT_AYUV, MFVideoFormat_AYUV},
		{SubType::VIDEO_FORMAT_YUY2, MFVideoFormat_YUY2},
		{SubType::VIDEO_FORMAT_YVYU, MFVideoFormat_YVYU},
		{SubType::VIDEO_FORMAT_YVU9, MFVideoFormat_YVU9},
		{SubType::VIDEO_FORMAT_UYVY, MFVideoFormat_UYVY},
		{SubType::VIDEO_FORMAT_NV11, MFVideoFormat_NV11},
		{SubType::VIDEO_FORMAT_NV12, MFVideoFormat_NV12},
		{SubType::VIDEO_FORMAT_NV21, MFVideoFormat_NV21},
		{SubType::VIDEO_FORMAT_YV12, MFVideoFormat_YV12},
		{SubType::VIDEO_FORMAT_I420, MFVideoFormat_I420},
		{SubType::VIDEO_FORMAT_IYUV, MFVideoFormat_IYUV},
		{SubType::VIDEO_FORMAT_Y210, MFVideoFormat_Y210},
		{SubType::VIDEO_FORMAT_Y216, MFVideoFormat_Y216},
		{SubType::VIDEO_FORMAT_Y410, MFVideoFormat_Y410},
		{SubType::VIDEO_FORMAT_Y416, MFVideoFormat_Y416},
		{SubType::VIDEO_FORMAT_Y41P, MFVideoFormat_Y41P},
		{SubType::VIDEO_FORMAT_Y41T, MFVideoFormat_Y41T},
		{SubType::VIDEO_FORMAT_Y42T, MFVideoFormat_Y42T},
		{SubType::VIDEO_FORMAT_P210, MFVideoFormat_P210},
		{SubType::VIDEO_FORMAT_P216, MFVideoFormat_P216},
		{SubType::VIDEO_FORMAT_P010, MFVideoFormat_P010},
		{SubType::VIDEO_FORMAT_P016, MFVideoFormat_P016},
		{SubType::VIDEO_FORMAT_v210, MFVideoFormat_v210},
		{SubType::VIDEO_FORMAT_v216, MFVideoFormat_v216},
		{SubType::VIDEO_FORMAT_v410, MFVideoFormat_v410},
		{SubType::VIDEO_FORMAT_MP43, MFVideoFormat_MP43},
		{SubType::VIDEO_FORMAT_MP4S, MFVideoFormat_MP4S},
		{SubType::VIDEO_FORMAT_M4S2, MFVideoFormat_M4S2},
		{SubType::VIDEO_FORMAT_MP4V, MFVideoFormat_MP4V},
		{SubType::VIDEO_FORMAT_WMV1, MFVideoFormat_WMV1},
		{SubType::VIDEO_FORMAT_WMV2, MFVideoFormat_WMV2},
		{SubType::VIDEO_FORMAT_WMV3, MFVideoFormat_WMV3},
		{SubType::VIDEO_FORMAT_WVC1, MFVideoFormat_WVC1},
		{SubType::VIDEO_FORMAT_MSS1, MFVideoFormat_MSS1},
		{SubType::VIDEO_FORMAT_MSS2, MFVideoFormat_MSS2},
		{SubType::VIDEO_FORMAT_MPG1, MFVideoFormat_MPG1},
		{SubType::VIDEO_FORMAT_DVSL, MFVideoFormat_DVSL},
		{SubType::VIDEO_FORMAT_DVSD, MFVideoFormat_DVSD},
		{SubType::VIDEO_FORMAT_DVHD, MFVideoFormat_DVHD},
		{SubType::VIDEO_FORMAT_DV25, MFVideoFormat_DV25},
		{SubType::VIDEO_FORMAT_DV50, MFVideoFormat_DV50},
		{SubType::VIDEO_FORMAT_DVH1, MFVideoFormat_DVH1},
		{SubType::VIDEO_FORMAT_DVC, MFVideoFormat_DVC},
		{SubType::VIDEO_FORMAT_H264, MFVideoFormat_H264},
		{SubType::VIDEO_FORMAT_H265, MFVideoFormat_H265},
		{SubType::VIDEO_FORMAT_MJPG, MFVideoFormat_MJPG},
		{SubType::VIDEO_FORMAT_420O, MFVideoFormat_420O},
		{SubType::VIDEO_FORMAT_HEVC, MFVideoFormat_HEVC},
		{SubType::VIDEO_FORMAT_HEVC_ES, MFVideoFormat_HEVC_ES},
		{SubType::VIDEO_FORMAT_VP80, MFVideoFormat_VP80},
		{SubType::VIDEO_FORMAT_VP90, MFVideoFormat_VP90},
		{SubType::VIDEO_FORMAT_ORAW, MFVideoFormat_ORAW},
		{SubType::VIDEO_FORMAT_H263, MFVideoFormat_H263},
		{SubType::VIDEO_FORMAT_A2R10G10B10, MFVideoFormat_A2R10G10B10},
		{SubType::VIDEO_FORMAT_A16B16G16R16F, MFVideoFormat_A16B16G16R16F},
		{SubType::VIDEO_FORMAT_VP10, MFVideoFormat_VP10},
		{SubType::VIDEO_FORMAT_AV1, MFVideoFormat_AV1},
		{SubType::VIDEO_FORMAT_Theora, MFVideoFormat_Theora},
	};
	return MAP;
}

MediaFoundation::MediaType MediaFoundation::MediaType::get_best_media_type(std::vector<MediaType> media_types)
{
	static constexpr std::array FORMATS_PRIORITY = {
		SubType::VIDEO_FORMAT_RGB32,
		SubType::VIDEO_FORMAT_ARGB32,
		SubType::VIDEO_FORMAT_MJPG,
		SubType::VIDEO_FORMAT_YUY2,
		SubType::VIDEO_FORMAT_M4S2,
		SubType::VIDEO_FORMAT_MPG1,
		SubType::VIDEO_FORMAT_MP43,
		SubType::VIDEO_FORMAT_MP4S,
		SubType::VIDEO_FORMAT_MP4V,
		SubType::VIDEO_FORMAT_HEVC,
		SubType::VIDEO_FORMAT_HEVC_ES,
		SubType::VIDEO_FORMAT_H263,
		SubType::VIDEO_FORMAT_H264,
		SubType::VIDEO_FORMAT_H265,
		SubType::VIDEO_FORMAT_RGB24,
		SubType::VIDEO_FORMAT_RGB555,
		SubType::VIDEO_FORMAT_RGB565,
		SubType::VIDEO_FORMAT_RGB8,
		SubType::VIDEO_FORMAT_A2R10G10B10,
	};

	filter_best_resolution(media_types);

	for (SubType sub_type : FORMATS_PRIORITY)
	{
		auto it = std::ranges::find_if(
			media_types,
			[sub_type](const MediaType& media_type)
			{
				return media_type.get_sub_type() == sub_type;
			}
		);

		if (it == media_types.end())
		{
			continue;
		}

		return std::move(*it);
	}

	return std::move(*media_types.begin());
}

void MediaFoundation::MediaType::filter_best_resolution(std::vector<MediaType>& media_types)
{
	if (media_types.empty())
	{
		return;
	}

	auto compare = [](const MediaType& left, const MediaType& right)
	{
		return left.get_dimensions().area() < right.get_dimensions().area();
	};

	const auto largest_resolution_iterator = std::ranges::max_element(media_types, compare);
	MediaType largest_resolution = std::move(*largest_resolution_iterator);
	media_types.erase(largest_resolution_iterator);

	std::erase_if(
		media_types,
		[&largest_resolution, &compare](const MediaType& elem)
		{
			return compare(elem, largest_resolution) || compare(largest_resolution, elem);
		}
	);

	media_types.push_back(std::move(largest_resolution));
}

std::wstring MediaFoundation::to_wstring(const MediaType::SubType sub_type)
{
	switch (sub_type)
	{
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_RGB32);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_ARGB32);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_RGB24);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_RGB555);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_RGB565);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_RGB8);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_L8);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_L16);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_D16);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_AI44);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_AYUV);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_YUY2);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_YVYU);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_YVU9);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_UYVY);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_NV11);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_NV12);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_NV21);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_YV12);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_I420);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_IYUV);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_Y210);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_Y216);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_Y410);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_Y416);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_Y41P);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_Y41T);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_Y42T);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_P210);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_P216);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_P010);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_P016);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_v210);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_v216);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_v410);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_MP43);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_MP4S);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_M4S2);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_MP4V);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_WMV1);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_WMV2);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_WMV3);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_WVC1);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_MSS1);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_MSS2);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_MPG1);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_DVSL);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_DVSD);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_DVHD);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_DV25);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_DV50);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_DVH1);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_DVC);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_H264);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_H265);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_MJPG);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_420O);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_HEVC);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_HEVC_ES);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_VP80);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_VP90);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_ORAW);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_H263);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_A2R10G10B10);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_A16B16G16R16F);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_VP10);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_AV1);
	DEFINE_ENUM_FORMAT_CASE(MediaType::SubType::VIDEO_FORMAT_Theora);

	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}
