#pragma once
#include "MediaBuffer.hpp"
#include "Com/Releaser.hpp"

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
	Sample(Sample&&) = default;
	Sample& operator=(Sample&&) = default;

	friend class SourceReader;

	[[nodiscard]] MediaBuffer to_contiguous_buffer() const;

private:
	Com::Releaser m_sample;

	[[nodiscard]] IMFSample* get() const;
};
}
