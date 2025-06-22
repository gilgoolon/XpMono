#include "Com/ComRuntime.hpp"

#include "Trace.hpp"
#include "Com/ComException.hpp"
#include "Synchronization/CriticalSection.hpp"

ComRuntime::ComRuntime()
{
	static constexpr LPVOID RESERVED = nullptr;
	HRESULT hresult = CoInitializeEx(RESERVED, COINIT_MULTITHREADED);
	if (FAILED(hresult))
	{
		throw ComException(ErrorCode::FAILED_COM_INITIALIZE, hresult);
	}
	static constexpr long AUTO_SELECT_AUTH_SERVICE = -1;
	hresult = CoInitializeSecurity(
		nullptr,
		AUTO_SELECT_AUTH_SERVICE,
		nullptr,
		RESERVED,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr,
		EOAC_NONE,
		RESERVED
	);
	if (FAILED(hresult))
	{
		CoUninitialize();
		throw ComException(ErrorCode::FAILED_COM_INITIALIZE_SECURITY, hresult);
	}
}

ComRuntime::~ComRuntime()
{
	try
	{
		CoUninitialize();
	}
	catch (...)
	{
		TRACE(L"failed to uninitialize com");
	}
}
