#pragma once
#include <memory>
#include <Windows.h>

struct LocalAllocationFreer final
{
	void operator()(HLOCAL address) const;
};

using ScopedLocalHeapAllocation = std::unique_ptr<std::remove_pointer_t<HLOCAL>, LocalAllocationFreer>;
