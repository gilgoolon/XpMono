#include "Com/ComException.hpp"
#include <comdef.h>

ComException::ComException(const ErrorCode error_code, const HRESULT hresult):
	Exception(error_code),
	m_hresult(hresult)
{
}

HRESULT ComException::hresult() const
{
	return m_hresult;
}

std::string ComException::message() const
{
	const _com_error err(m_hresult);
	return err.ErrorMessage();
}
