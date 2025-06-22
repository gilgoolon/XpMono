#include "Com/ComReleaser.hpp"

#include "Trace.hpp"

void ComReleaserDeleter::operator()(IUnknown* const object) const
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
