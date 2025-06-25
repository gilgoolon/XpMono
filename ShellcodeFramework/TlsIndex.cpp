#include "TlsIndex.hpp"

#include "Trace.hpp"

#include <Windows.h>

TlsIndex::TlsIndex(bool& result):
	m_index(),
	m_is_initialized(allocate_index(m_index))
{
	result = m_is_initialized;
}

TlsIndex::~TlsIndex()
{
	if (!m_is_initialized)
	{
		return;
	}

	if (TlsFree(m_index) == FALSE)
	{
		TRACE(L"failed to free TLS index");
	}
}

bool TlsIndex::set_value(void* value)
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
