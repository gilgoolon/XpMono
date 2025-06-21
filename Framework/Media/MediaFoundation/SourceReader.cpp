#include "Media/MediaFoundation/SourceReader.hpp"

#include "Wmi/WmiException.hpp"

#include <mfapi.h>

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

void MediaFoundation::SourceReader::set_media_type(const MediaType& media_type)
{
	static constexpr DWORD* RESERVED = nullptr;
	static constexpr DWORD FIRST_VIDEO_STREAM = stream();

	const HRESULT result = get()->SetCurrentMediaType(
		FIRST_VIDEO_STREAM,
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

	return sample == WAITING_FOR_SAMPLE ? std::nullopt : Sample(sample);
}
