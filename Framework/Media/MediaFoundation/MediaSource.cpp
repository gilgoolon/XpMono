#include "Media/MediaFoundation/MediaSource.hpp"

#include "Wmi/WmiException.hpp"

MediaFoundation::MediaSource::MediaSource(IMFMediaSource* source):
	m_source(source)
{
}

MediaFoundation::SourceReader MediaFoundation::MediaSource::create_reader()
{
	IMFSourceReader* source_reader = nullptr;

	const HRESULT result = MFCreateSourceReaderFromMediaSource(get(), nullptr, &source_reader);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_CREATE_READER, result);
	}

	return SourceReader(source_reader);
}

IMFMediaSource* MediaFoundation::MediaSource::get() const
{
	return reinterpret_cast<IMFMediaSource*>(m_source.get());
}
