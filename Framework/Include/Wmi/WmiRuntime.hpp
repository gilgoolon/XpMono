#pragma once

class WmiRuntime final
{
public:
	explicit WmiRuntime();
	~WmiRuntime();
	WmiRuntime(const WmiRuntime&) = delete;
	WmiRuntime& operator=(const WmiRuntime&) = delete;
	WmiRuntime(WmiRuntime&&) = delete;
	WmiRuntime& operator=(WmiRuntime&&) = delete;
};
