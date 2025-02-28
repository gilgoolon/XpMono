#include "Wmi/WmiConnection.hpp"

#include "Trace.hpp"
#include "Wmi/WmiException.hpp"
#include "Wmi/WmiVariant.hpp"

#include <comutil.h>

WmiConnection::WmiConnection(const std::wstring& namespace_path):
	m_runtime(),
	m_locator(create_locator()),
	m_services(connect_server(static_cast<IWbemLocator*>(m_locator.get()), namespace_path))
{
	const HRESULT hresult = CoSetProxyBlanket(
		m_services.get(),
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		nullptr,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr,
		EOAC_NONE
	);
	if (FAILED(hresult))
	{
		throw WmiException(ErrorCode::FAILED_WMI_SET_PROXY, hresult);
	}
}

std::vector<std::unique_ptr<WmiResult>> WmiConnection::query(const std::wstring& query) const
{
	static constexpr auto WMI_QUERY_LANGUAGE = "WQL";
	static constexpr long SYNC_QUERY = WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY;
	static constexpr IWbemContext* NO_CONTEXT = nullptr;
	IEnumWbemClassObject* enumerator = nullptr;
	HRESULT hresult = static_cast<IWbemServices*>(m_services.get())->ExecQuery(
		bstr_t(WMI_QUERY_LANGUAGE),
		bstr_t(query.c_str()),
		SYNC_QUERY,
		NO_CONTEXT,
		&enumerator
	);
	if (FAILED(hresult))
	{
		throw WmiException(ErrorCode::FAILED_WMI_QUERY, hresult);
	}
	WmiReleaser enumerator_releaser(enumerator);
	std::vector<std::unique_ptr<WmiResult>> results;
	while (enumerator)
	{
		IWbemClassObject* object = nullptr;
		ULONG uReturn = 0;
		hresult = enumerator->Next(WBEM_INFINITE, 1, &object, &uReturn);
		if (FAILED(hresult))
		{
			throw WmiException(ErrorCode::FAILED_WMI_ENUMERATE_RESULT, hresult);
		}
		if (uReturn == 0) break;
		WmiReleaser object_releaser(object);
		results.push_back(std::make_unique<WmiResult>(object));
	}
	return results;
}

IWbemLocator* WmiConnection::create_locator()
{
	IWbemLocator* locator = nullptr;
	const HRESULT result = CoCreateInstance(
		CLSID_WbemLocator,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		reinterpret_cast<LPVOID*>(&locator)
	);
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_WMI_CREATE_INSTANCE, result);
	}
	return locator;
}

IWbemServices* WmiConnection::connect_server(IWbemLocator* locator, const std::wstring& namespace_path)
{
	static constexpr BSTR NO_USER = nullptr;
	static constexpr BSTR NO_PASSWORD = nullptr;
	static constexpr BSTR DEFAULT_LOCALE = nullptr;
	static constexpr long DEFAULT_SECURITY = 0;
	static constexpr BSTR DEFAULT_AUTHORITY = nullptr;
	static constexpr IWbemContext* DEFAULT_CONTEXT = nullptr;

	IWbemServices* result = nullptr;
	const HRESULT hresult = locator->ConnectServer(
		bstr_t(namespace_path.c_str()),
		NO_USER,
		NO_PASSWORD,
		DEFAULT_LOCALE,
		DEFAULT_SECURITY,
		DEFAULT_AUTHORITY,
		DEFAULT_CONTEXT,
		&result
	);
	if (FAILED(hresult))
	{
		throw WmiException(ErrorCode::FAILED_WMI_CONNECT_SERVER, hresult);
	}
	return result;
}
