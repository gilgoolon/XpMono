#include "Wmi/ComTaskMemoryReleaser.hpp"

#include "Trace.hpp"

void ComTaskMemoryFreer::operator()(const PVOID memory) const
{
	try
	{
		CoTaskMemFree(memory);
	}
	catch (...)
	{
		TRACE(L"failed to free com task memory");
	}
}
