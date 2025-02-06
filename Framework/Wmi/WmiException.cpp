#include "Wmi/WmiException.hpp"
#include <comdef.h>

WmiException::WmiException(const ErrorCode error_code, const HRESULT hresult):
	Exception(error_code),
	m_hresult(hresult)
{
}

HRESULT WmiException::hresult() const
{
	return m_hresult;
}

std::string WmiException::message() const
{
	const _com_error err(m_hresult);
	return err.ErrorMessage();
}
