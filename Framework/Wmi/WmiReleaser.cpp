#include "Wmi/WmiReleaser.hpp"

#include "Trace.hpp"

void WmiReleaserDeleter::operator()(IUnknown* const object) const
{
	try
	{
		object->Release();
	}
	catch (...)
	{
		TRACE(L"failed to release wmi object");
	}
}
