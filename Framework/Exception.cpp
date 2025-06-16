#include <WinSock2.h>

#include "Exception.hpp"

#include <Windows.h>

Exception::Exception(const ErrorCode code):
	m_code(code)
{
}

ErrorCode Exception::code() const
{
	return m_code;
}

bool Exception::operator==(const ErrorCode error_code) const
{
	return m_code == error_code;
}

WinApiException::WinApiException(const ErrorCode code):
	WinApiException(code, GetLastError())
{
}

WinApiException::WinApiException(const ErrorCode code, const DWORD error):
	Exception(code),
	m_error(error)
{
}

uint32_t WinApiException::error() const
{
	return m_error;
}

WinApiNtException::WinApiNtException(const ErrorCode code, const NTSTATUS status):
	Exception(code),
	m_status(status)
{
}

NTSTATUS WinApiNtException::status() const
{
	return m_status;
}

GenericFunctionException::GenericFunctionException(const uint32_t function_code):
	Exception(ErrorCode::GENERIC_FUNCTION_EXCEPTION),
	m_function_code(function_code)
{
}

uint32_t GenericFunctionException::function_code() const
{
	return m_function_code;
}

WsaException::WsaException(const ErrorCode code):
	Exception(code),
	m_wsa_code(WSAGetLastError())
{
}

uint32_t WsaException::wsa_code() const
{
	return m_wsa_code;
}
