#include "Com/ComSafeArrayAccess.hpp"

#include "Trace.hpp"
#include "Com/ComException.hpp"

ComSafeArrayAccess::ComSafeArrayAccess(SAFEARRAY* safe_array):
	m_safearray(safe_array),
	m_data(nullptr)
{
	if (m_safearray == nullptr)
	{
		return;
	}
	const HRESULT hresult = SafeArrayAccessData(m_safearray, &m_data);
	if (FAILED(hresult))
	{
		throw ComException(ErrorCode::FAILED_COM_SAFE_ARRAY_ACCESS, hresult);
	}
}

ComSafeArrayAccess::~ComSafeArrayAccess()
{
	try
	{
		if (m_safearray == nullptr)
		{
			return;
		}
		if (FAILED(SafeArrayUnaccessData(m_safearray)))
		{
			TRACE(L"failed to unaccess safe array");
		}
	}
	catch (...)
	{
		TRACE(L"failed to unaccess safe array");
	}
}
