#pragma once
#include <memory>
#include <Windows.h>

struct ComTaskMemoryFreer final
{
	void operator()(PVOID memory) const;
};

using ComTaskMemoryReleaser = std::unique_ptr<std::remove_pointer_t<PVOID>, ComTaskMemoryFreer>;
