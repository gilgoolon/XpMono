#pragma once
#include "Utils/BiMap.hpp"
#include "Wmi/WmiReleaser.hpp"

#include <cstdint>
#include <mfobjects.h>

namespace MediaFoundation
{
class MediaType final
{
	explicit MediaType(IMFMediaType* media_type);

public:
	enum class Type : uint32_t
	{
		VIDEO = 0,
		AUDIO,
	};

	enum class SubType : uint32_t
	{
		VIDEO_FORMAT_RGB32 = 0,
		VIDEO_FORMAT_ARGB32,
		VIDEO_FORMAT_RGB24,
		VIDEO_FORMAT_RGB555,
		VIDEO_FORMAT_RGB565,
		VIDEO_FORMAT_RGB8,
		VIDEO_FORMAT_L8,
		VIDEO_FORMAT_L16,
		VIDEO_FORMAT_D16,
		VIDEO_FORMAT_AI44,
		VIDEO_FORMAT_AYUV,
		VIDEO_FORMAT_YUY2,
		VIDEO_FORMAT_YVYU,
		VIDEO_FORMAT_YVU9,
		VIDEO_FORMAT_UYVY,
		VIDEO_FORMAT_NV11,
		VIDEO_FORMAT_NV12,
		VIDEO_FORMAT_NV21,
		VIDEO_FORMAT_YV12,
		VIDEO_FORMAT_I420,
		VIDEO_FORMAT_IYUV,
		VIDEO_FORMAT_Y210,
		VIDEO_FORMAT_Y216,
		VIDEO_FORMAT_Y410,
		VIDEO_FORMAT_Y416,
		VIDEO_FORMAT_Y41P,
		VIDEO_FORMAT_Y41T,
		VIDEO_FORMAT_Y42T,
		VIDEO_FORMAT_P210,
		VIDEO_FORMAT_P216,
		VIDEO_FORMAT_P010,
		VIDEO_FORMAT_P016,
		VIDEO_FORMAT_v210,
		VIDEO_FORMAT_v216,
		VIDEO_FORMAT_v410,
		VIDEO_FORMAT_MP43,
		VIDEO_FORMAT_MP4S,
		VIDEO_FORMAT_M4S2,
		VIDEO_FORMAT_MP4V,
		VIDEO_FORMAT_WMV1,
		VIDEO_FORMAT_WMV2,
		VIDEO_FORMAT_WMV3,
		VIDEO_FORMAT_WVC1,
		VIDEO_FORMAT_MSS1,
		VIDEO_FORMAT_MSS2,
		VIDEO_FORMAT_MPG1,
		VIDEO_FORMAT_DVSL,
		VIDEO_FORMAT_DVSD,
		VIDEO_FORMAT_DVHD,
		VIDEO_FORMAT_DV25,
		VIDEO_FORMAT_DV50,
		VIDEO_FORMAT_DVH1,
		VIDEO_FORMAT_DVC,
		VIDEO_FORMAT_H264,
		VIDEO_FORMAT_H265,
		VIDEO_FORMAT_MJPG,
		VIDEO_FORMAT_420O,
		VIDEO_FORMAT_HEVC,
		VIDEO_FORMAT_HEVC_ES,
		VIDEO_FORMAT_VP80,
		VIDEO_FORMAT_VP90,
		VIDEO_FORMAT_ORAW,
		VIDEO_FORMAT_H263,
		VIDEO_FORMAT_A2R10G10B10,
		VIDEO_FORMAT_A16B16G16R16F,
		VIDEO_FORMAT_VP10,
		VIDEO_FORMAT_AV1,
		VIDEO_FORMAT_Theora,
	};

	explicit MediaType(Type type, SubType sub_type);
	~MediaType() = default;
	MediaType(const MediaType&) = delete;
	MediaType& operator=(const MediaType&) = delete;
	MediaType(MediaType&&) = default;
	MediaType& operator=(MediaType&&) = default;

	struct Dimensions
	{
		uint32_t width;
		uint32_t height;

		[[nodiscard]] uint64_t area() const;
	};

	[[nodiscard]] Dimensions get_dimensions() const;
	[[nodiscard]] SubType get_sub_type() const;

	friend class SourceReader;

private:
	WmiReleaser m_type;

	[[nodiscard]] IMFMediaType* get() const;
	[[nodiscard]] GUID get_guid(const GUID& key) const;
	void set_guid(const GUID& key, const GUID& value);

	[[nodiscard]] static IMFMediaType* create_media_type();
	[[nodiscard]] static GUID get_type_guid(Type type);

	[[nodiscard]] static GUID get_sub_type_guid(SubType sub_type);
	[[nodiscard]] static SubType get_sub_type_enum(const GUID& sub_type);

	[[nodiscard]] static const BiMap<SubType, GUID>& sub_type_map();

public:
	[[nodiscard]] static MediaType get_best_media_type(std::vector<MediaType> media_types);
	static void filter_best_resolution(std::vector<MediaType>& media_types);
};

[[nodiscard]] std::wstring to_wstring(MediaType::SubType sub_type);
}
