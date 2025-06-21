#pragma once
#include "Utils/Buffer.hpp"
#include "Wmi/WmiReleaser.hpp"

#include <mfobjects.h>

namespace MediaFoundation
{
class MediaBuffer final
{
public:
	explicit MediaBuffer(IMFMediaBuffer* media_buffer);
	~MediaBuffer() = default;
	MediaBuffer(const MediaBuffer&) = delete;
	MediaBuffer& operator=(const MediaBuffer&) = delete;
	MediaBuffer(MediaBuffer&&) = delete;
	MediaBuffer& operator=(MediaBuffer&&) = delete;

	[[nodiscard]] Buffer to_buffer() const;

private:
	WmiReleaser m_media_buffer;

	[[nodiscard]] IMFMediaBuffer* get() const;

	struct Unlocker
	{
		void operator()(IMFMediaBuffer* buffer) const;
	};

	using LockGuard = std::unique_ptr<IMFMediaBuffer, Unlocker>;

	using UnlockedBuffer = std::tuple<LockGuard, Buffer>;
	[[nodiscard]] UnlockedBuffer lock() const;
};
}
