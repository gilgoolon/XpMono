#pragma once
#include "MediaBuffer.hpp"
#include "Wmi/WmiReleaser.hpp"

#include <mfobjects.h>

namespace MediaFoundation
{
class Sample final
{
	explicit Sample(IMFSample* sample);

public:
	~Sample() = default;
	Sample(const Sample&) = delete;
	Sample& operator=(const Sample&) = delete;
	Sample(Sample&&) = delete;
	Sample& operator=(Sample&&) = delete;

	friend class SourceReader;

	[[nodiscard]] MediaBuffer to_contiguous_buffer() const;

private:
	WmiReleaser m_sample;

	[[nodiscard]] IMFSample* get() const;
};
}
