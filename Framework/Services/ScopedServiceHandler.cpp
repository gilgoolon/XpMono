#include "Services/ScopedServiceHandler.hpp"
#include "Trace.hpp"

void ServiceHandleCloser::operator()(const SC_HANDLE handle) const
{
	try
	{
		if (CloseServiceHandle(handle) == FALSE)
		{
			TRACE(L"failed to close handle");
		}
	}
	catch (...)
	{
		TRACE(L"failed to close handle");
	}
}
