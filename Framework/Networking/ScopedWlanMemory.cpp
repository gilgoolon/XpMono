#include "Networking/ScopedWlanMemory.hpp"

#include "Trace.hpp"

#include <wlanapi.h>

void WlanMemoryFreer::operator()(const PVOID wlan_allocated_memory) const
{
	try
	{
		if (wlan_allocated_memory != nullptr)
		{
			WlanFreeMemory(wlan_allocated_memory);
		}
	}
	catch (...)
	{
		TRACE(L"failed to free wlan allocated memory");
	}
}
