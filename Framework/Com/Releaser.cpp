#include "Com/Releaser.hpp"

#include "Trace.hpp"

void Com::ReleaserDeleter::operator()(IUnknown* const object) const
{
	try
	{
		object->Release();
	}
	catch (...)
	{
		TRACE(L"failed to release com object");
	}
}
