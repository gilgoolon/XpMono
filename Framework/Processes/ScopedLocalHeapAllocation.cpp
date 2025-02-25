#include "Processes/ScopedLocalHeapAllocation.hpp"

#include "Trace.hpp"

void LocalAllocationFreer::operator()(const HLOCAL address) const
{
	try
	{
		if (LocalFree(address) != nullptr)
		{
			TRACE(L"failed to free local allocation");
		}
	}
	catch (...)
	{
		TRACE(L"failed to free local allocation");
	}
}
