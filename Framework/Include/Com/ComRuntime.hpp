#pragma once

class ComRuntime final
{
public:
	explicit ComRuntime();
	~ComRuntime();
	ComRuntime(const ComRuntime&) = delete;
	ComRuntime& operator=(const ComRuntime&) = delete;
	ComRuntime(ComRuntime&&) = delete;
	ComRuntime& operator=(ComRuntime&&) = delete;
};
