#pragma once
#include <memory>
#include <Windows.h>

struct WlanMemoryFreer final
{
	void operator()(PVOID wlan_allocated_memory) const;
};

using ScopedWlanMemory = std::unique_ptr<std::remove_pointer_t<PVOID>, WlanMemoryFreer>;
