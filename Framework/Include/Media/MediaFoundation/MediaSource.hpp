#pragma once
#include "SourceReader.hpp"
#include "Wmi/WmiReleaser.hpp"

#include <mfidl.h>

namespace MediaFoundation
{
class MediaSource final
{
	explicit MediaSource(IMFMediaSource* source);

public:
	~MediaSource() = default;
	MediaSource(const MediaSource&) = delete;
	MediaSource& operator=(const MediaSource&) = delete;
	MediaSource(MediaSource&&) = default;
	MediaSource& operator=(MediaSource&&) = default;

	friend class Device;

	[[nodiscard]] SourceReader create_reader();

private:
	WmiReleaser m_source;

	[[nodiscard]] IMFMediaSource* get() const;
};
}
