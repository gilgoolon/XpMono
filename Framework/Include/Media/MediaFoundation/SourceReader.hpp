#pragma once
#include "MediaType.hpp"
#include "Sample.hpp"
#include "Wmi/WmiReleaser.hpp"

#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mfreadwrite.h>
#include <optional>

namespace MediaFoundation
{
class SourceReader final
{
	explicit SourceReader(IMFSourceReader* reader);

public:
	~SourceReader() = default;
	SourceReader(const SourceReader&) = delete;
	SourceReader& operator=(const SourceReader&) = delete;
	SourceReader(SourceReader&&) = delete;
	SourceReader& operator=(SourceReader&&) = delete;

	friend class MediaSource;

	[[nodiscard]] std::vector<MediaType> get_supported_media_types() const;
	void set_media_type(const MediaType& media_type);
	[[nodiscard]] std::optional<Sample> read_sample() const;

private:
	WmiReleaser m_reader;

	[[nodiscard]] IMFSourceReader* get() const;

	[[nodiscard]] static DWORD stream();

	[[nodiscard]] std::optional<MediaType> get_media_type(uint32_t index) const;
};
}
