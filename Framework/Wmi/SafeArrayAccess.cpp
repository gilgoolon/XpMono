#include "Wmi/SafeArrayAccess.hpp"

#include "Trace.hpp"
#include "Wmi/WmiException.hpp"

SafeArrayAccess::SafeArrayAccess(SAFEARRAY* safe_array):
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
		throw WmiException(ErrorCode::FAILED_SAFE_ARRAY_ACCESS, hresult);
	}
}

SafeArrayAccess::~SafeArrayAccess()
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
