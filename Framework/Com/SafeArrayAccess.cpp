#include "Com/SafeArrayAccess.hpp"

#include "Trace.hpp"

Com::SafeArrayAccess::SafeArrayAccess(SAFEARRAY* safe_array):
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

Com::SafeArrayAccess::~SafeArrayAccess()
{
	try
	{
		if (m_safearray == nullptr)
		{
			return;
		}
		if (FAILED(SafeArrayUnaccessData(m_safearray)))
		{
			TRACE(L"failed to unaccess com safe array");
		}
	}
	catch (...)
	{
		TRACE(L"failed to unaccess com safe array");
	}
}
