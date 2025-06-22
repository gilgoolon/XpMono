#include "Media/MediaFoundation/Instance.hpp"

#include "Trace.hpp"
#include "Com/ComException.hpp"

#include <mfapi.h>

MediaFoundation::Instance::Instance()
{
	const HRESULT result = MFStartup(MF_VERSION);

	if (FAILED(result))
	{
		throw ComException(ErrorCode::FAILED_MEDIA_FOUNDATION_STARTUP, result);
	}
}

MediaFoundation::Instance::~Instance()
{
	try
	{
		const HRESULT result = MFShutdown();
		if (FAILED(result))
		{
			TRACE(L"failed to shutdown MediaFoundation instance with hresult ", result);
		}
	}
	catch (...)
	{
		TRACE(L"failed to shutdown MediaFoundation instance");
	}
}
