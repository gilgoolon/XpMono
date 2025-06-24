#include "TlsIndex.hpp"

#include "SafeTrace.hpp"

#include <Windows.h>

TlsIndex::TlsIndex(bool& result):
	m_index(),
	m_initialized(allocate_index(m_index))
{
}

TlsIndex::~TlsIndex()
{
	if (!m_initialized)
	{
		return;
	}

	if (TlsFree(m_index) == FALSE)
	{
		TRACE(L"failed to free TLS index");
	}
}

bool TlsIndex::set_value(uint8_t* value)
{
	const BOOL result = TlsSetValue(m_index, value);

	if (result == FALSE)
	{
		TRACE(L"failed to set tls index value");
		return false;
	}

	return true;
}

bool TlsIndex::allocate_index(uint32_t& index)
{
	const DWORD result = TlsAlloc();

	if (result == TLS_OUT_OF_INDEXES)
	{
		TRACE(L"failed TlsAlloc");
		return false;
	}

	index = result;
	return true;
}
