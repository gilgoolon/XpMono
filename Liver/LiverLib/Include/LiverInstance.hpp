#pragma once
#include "Synchronization/Mutex.hpp"

class LiverInstance final
{
public:
	explicit LiverInstance();
	~LiverInstance() = default;
	LiverInstance(const LiverInstance&) = delete;
	LiverInstance& operator=(const LiverInstance&) = delete;
	LiverInstance(LiverInstance&&) = delete;
	LiverInstance& operator=(LiverInstance&&) = delete;

private:
	Mutex m_mutex;
};
