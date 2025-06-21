#include "Media/MediaFoundation/Sample.hpp"

#include "Wmi/WmiException.hpp"

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
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_TO_CONTIGUOUS_BUFFER, result);
	}

	return MediaBuffer(media_buffer);
}
