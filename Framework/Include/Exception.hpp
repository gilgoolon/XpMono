#pragma once
#include "ErrorCode.hpp"

#include <cstdint>
#include <Windows.h>

class Exception
{
public:
	explicit Exception(ErrorCode code);
	virtual ~Exception() = default;
	Exception(const Exception&) = delete;
	Exception& operator=(const Exception&) = delete;
	Exception(Exception&&) = delete;
	Exception& operator=(Exception&&) = delete;

	[[nodiscard]] ErrorCode code() const;

	[[nodiscard]] bool operator==(ErrorCode error_code) const;

private:
	ErrorCode m_code;
};

class WinApiException final : public Exception
{
public:
	explicit WinApiException(ErrorCode code);
	explicit WinApiException(ErrorCode code, DWORD error);
	~WinApiException() override = default;
	WinApiException(const WinApiException&) = delete;
	WinApiException& operator=(const WinApiException&) = delete;
	WinApiException(WinApiException&&) = delete;
	WinApiException& operator=(WinApiException&&) = delete;

	[[nodiscard]] uint32_t error() const;

private:
	uint32_t m_error;
};

class WinApiNtException final : public Exception
{
public:
	explicit WinApiNtException(ErrorCode code, NTSTATUS status);
	~WinApiNtException() override = default;
	WinApiNtException(const WinApiNtException&) = delete;
	WinApiNtException& operator=(const WinApiNtException&) = delete;
	WinApiNtException(WinApiNtException&&) = delete;
	WinApiNtException& operator=(WinApiNtException&&) = delete;

	[[nodiscard]] NTSTATUS status() const;

private:
	NTSTATUS m_status;
};

class ComException final : public Exception
{
public:
	explicit ComException(ErrorCode error_code, HRESULT hresult);
	~ComException() override = default;
	ComException(const ComException&) = delete;
	ComException& operator=(const ComException&) = delete;
	ComException(ComException&&) = delete;
	ComException& operator=(ComException&&) = delete;

	[[nodiscard]] HRESULT hresult() const;
	[[nodiscard]] std::string message() const;

private:
	HRESULT m_hresult;
};

class CriticalException final
{
public:
	explicit CriticalException() = default;
	~CriticalException() = default;
	CriticalException(const CriticalException&) = delete;
	CriticalException& operator=(const CriticalException&) = delete;
	CriticalException(CriticalException&&) = delete;
	CriticalException& operator=(CriticalException&&) = delete;
};

class GenericFunctionException final : public Exception
{
public:
	explicit GenericFunctionException(uint32_t function_code);
	~GenericFunctionException() override = default;
	GenericFunctionException(const GenericFunctionException&) = delete;
	GenericFunctionException& operator=(const GenericFunctionException&) = delete;
	GenericFunctionException(GenericFunctionException&&) = delete;
	GenericFunctionException& operator=(GenericFunctionException&&) = delete;

	[[nodiscard]] uint32_t function_code() const;

private:
	uint32_t m_function_code;
};

class WsaException final : public Exception
{
public:
	explicit WsaException(ErrorCode code);
	~WsaException() override = default;
	WsaException(const WsaException&) = delete;
	WsaException& operator=(const WsaException&) = delete;
	WsaException(WsaException&&) = delete;
	WsaException& operator=(WsaException&&) = delete;

	[[nodiscard]] uint32_t wsa_code() const;

private:
	uint32_t m_wsa_code;
};
