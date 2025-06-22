#include "Com/TaskMemoryReleaser.hpp"

#include "Trace.hpp"

void Com::TaskMemoryFreer::operator()(const PVOID memory) const
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
