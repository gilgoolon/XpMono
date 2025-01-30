#include "Processes/ScopedHandle.hpp"

#include "Trace.hpp"

void HandleCloser::operator()(const HANDLE handle) const
{
	try
	{
		if (CloseHandle(handle) == FALSE)
		{
			TRACE(L"failed to close handle")
		}
	}
	catch (...)
	{
		TRACE(L"failed to close handle")
	}
}
