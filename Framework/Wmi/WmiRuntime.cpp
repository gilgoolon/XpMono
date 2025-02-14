#include "Wmi/WmiRuntime.hpp"

#include "Trace.hpp"
#include "Synchronization/CriticalSection.hpp"
#include "Wmi/WmiException.hpp"

WmiRuntime::WmiRuntime()
{
	static constexpr LPVOID RESERVED = nullptr;
	HRESULT hresult = CoInitializeEx(RESERVED, COINIT_MULTITHREADED);
	if (FAILED(hresult))
	{
		throw WmiException(ErrorCode::FAILED_WMI_INITIALIZE, hresult);
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
		throw WmiException(ErrorCode::FAILED_WMI_INITIALIZE_SECURITY, hresult);
	}
}

WmiRuntime::~WmiRuntime()
{
	try
	{
		CoUninitialize();
	}
	catch (...)
	{
		TRACE(L"failed to uninitialize wmi")
	}
}
