#pragma once

namespace Com
{
class Runtime final
{
public:
	explicit Runtime();
	~Runtime();
	Runtime(const Runtime&) = delete;
	Runtime& operator=(const Runtime&) = delete;
	Runtime(Runtime&&) = delete;
	Runtime& operator=(Runtime&&) = delete;
};
}
