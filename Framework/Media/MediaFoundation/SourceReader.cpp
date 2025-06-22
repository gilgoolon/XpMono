#include "Media/MediaFoundation/SourceReader.hpp"

#include "Trace.hpp"
#include "Wmi/WmiException.hpp"

#include <Mferror.h>

MediaFoundation::SourceReader::SourceReader(IMFSourceReader* const reader):
	m_reader(reader)
{
}

IMFSourceReader* MediaFoundation::SourceReader::get() const
{
	return reinterpret_cast<IMFSourceReader*>(m_reader.get());
}

DWORD MediaFoundation::SourceReader::stream()
{
	return static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM);
}

std::vector<MediaFoundation::MediaType> MediaFoundation::SourceReader::get_supported_media_types() const
{
	std::vector<MediaType> result;

	for (uint32_t i = 0;; ++i)
	{
		std::optional<MediaType> media_type = get_media_type(i);

		if (!media_type.has_value())
		{
			break;
		}

		result.emplace_back(*std::move(media_type));
	}

	return result;
}

std::optional<MediaFoundation::MediaType> MediaFoundation::SourceReader::get_media_type(const uint32_t index) const
{
	IMFMediaType* media_type = nullptr;

	const HRESULT result = get()->GetNativeMediaType(stream(), index, &media_type);

	if (FAILED(result))
	{
		if (result == MF_E_NO_MORE_TYPES)
		{
			return std::nullopt;
		}

		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_GET_MEDIA_TYPE, result);
	}

	return {MediaType(media_type)};
}

void MediaFoundation::SourceReader::set_media_type(const MediaType& media_type)
{
	static constexpr DWORD* RESERVED = nullptr;
	const DWORD first_video_stream = stream();

	const HRESULT result = get()->SetCurrentMediaType(
		first_video_stream,
		RESERVED,
		media_type.get()
	);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_SET_MEDIA_TYPE, result);
	}
}

std::optional<MediaFoundation::Sample> MediaFoundation::SourceReader::read_sample() const
{
	static constexpr DWORD REQUEST_SAMPLE = 0;
	DWORD actual_stream_index = 0;
	DWORD flags = 0;
	LONGLONG timestamp = 0;
	IMFSample* sample = nullptr;

	const HRESULT result = get()->ReadSample(
		stream(),
		REQUEST_SAMPLE,
		&actual_stream_index,
		&flags,
		&timestamp,
		&sample
	);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_READ_SAMPLE, result);
	}

	static constexpr IMFSample* WAITING_FOR_SAMPLE = nullptr;

	return sample == WAITING_FOR_SAMPLE ? std::nullopt : std::optional{Sample(sample)};
}
