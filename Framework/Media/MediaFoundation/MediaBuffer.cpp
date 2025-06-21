#include "Media/MediaFoundation/MediaBuffer.hpp"

#include "Trace.hpp"
#include "Wmi/WmiException.hpp"

MediaFoundation::MediaBuffer::MediaBuffer(IMFMediaBuffer* const media_buffer):
	m_media_buffer(media_buffer)
{
}

Buffer MediaFoundation::MediaBuffer::to_buffer() const
{
	auto [unlocker, buffer] = lock();

	return std::move(buffer);
}

IMFMediaBuffer* MediaFoundation::MediaBuffer::get() const
{
	return reinterpret_cast<IMFMediaBuffer*>(m_media_buffer.get());
}

void MediaFoundation::MediaBuffer::Unlocker::operator()(IMFMediaBuffer* buffer) const
{
	try
	{
		const HRESULT result = buffer->Unlock();
		if (FAILED(result))
		{
			TRACE(L"failed unlocking media buffer with hresult ", result);
		}
	}
	catch (...)
	{
		TRACE(L"failed unlocking media buffer");
	}
}

MediaFoundation::MediaBuffer::UnlockedBuffer MediaFoundation::MediaBuffer::lock() const
{
	BYTE* data = nullptr;
	DWORD data_size = 0;
	DWORD buffer_size = 0;

	const HRESULT result = get()->Lock(&data, &buffer_size, &data_size);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_BUFFER_LOCK, result);
	}

	LockGuard lock_guard(get());

	const auto start = reinterpret_cast<const uint8_t*>(data);
	return {std::move(lock_guard), Buffer{start, start + data_size}};
}
