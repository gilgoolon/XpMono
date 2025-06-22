#include "Media/MediaFoundation/Sample.hpp"

#include "Com/ComException.hpp"

MediaFoundation::Sample::Sample(IMFSample* sample):
	m_sample(sample)
{
}

IMFSample* MediaFoundation::Sample::get() const
{
	return reinterpret_cast<IMFSample*>(m_sample.get());
}

MediaFoundation::MediaBuffer MediaFoundation::Sample::to_contiguous_buffer() const
{
	IMFMediaBuffer* media_buffer = nullptr;

	const HRESULT result = get()->ConvertToContiguousBuffer(&media_buffer);
	if (FAILED(result))
	{
		throw ComException(ErrorCode::FAILED_MEDIA_FOUNDATION_TO_CONTIGUOUS_BUFFER, result);
	}

	return MediaBuffer(media_buffer);
}
