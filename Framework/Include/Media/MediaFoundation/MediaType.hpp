#pragma once
#include "Wmi/WmiReleaser.hpp"

#include <cstdint>
#include <mfobjects.h>

namespace MediaFoundation
{
class MediaType final
{
public:
	enum class Type : uint32_t
	{
		VIDEO = 0,
	};

	enum class SubType : uint32_t
	{
		VIDEO_FORMAT_RGB32 = 0,
	};

	explicit MediaType(Type type, SubType sub_type);
	~MediaType() = default;
	MediaType(const MediaType&) = delete;
	MediaType& operator=(const MediaType&) = delete;
	MediaType(MediaType&&) = delete;
	MediaType& operator=(MediaType&&) = delete;

	struct Dimensions
	{
		uint32_t width;
		uint32_t height;
	};

	[[nodiscard]] Dimensions get_dimensions() const;

	friend class SourceReader;

private:
	WmiReleaser m_type;

	[[nodiscard]] IMFMediaType* get() const;
	void set_guid(const GUID& key, const GUID& value);

	[[nodiscard]] static IMFMediaType* create_media_type();
	[[nodiscard]] static GUID get_type_guid(Type type);
	[[nodiscard]] static GUID get_sub_type_guid(SubType sub_type);
};
}
