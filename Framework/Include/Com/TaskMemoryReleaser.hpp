#pragma once
#include <memory>
#include <Windows.h>

namespace Com
{
struct TaskMemoryFreer final
{
	void operator()(PVOID memory) const;
};

using TaskMemoryReleaser = std::unique_ptr<std::remove_pointer_t<PVOID>, TaskMemoryFreer>;
}
