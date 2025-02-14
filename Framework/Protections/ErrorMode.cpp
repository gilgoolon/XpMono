#include "Protections/ErrorMode.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

static constexpr int SET_MODE_ERROR = -1;

Protections::ErrorMode::ErrorMode():
	m_previous_mode(_set_error_mode(_OUT_TO_STDERR))
{
	if (m_previous_mode == SET_MODE_ERROR)
	{
		TRACE(L"failed to set error mode")
		throw CriticalException();
	}
}

Protections::ErrorMode::~ErrorMode()
{
	try
	{
		if (_set_error_mode(m_previous_mode) == SET_MODE_ERROR)
		{
			TRACE(L"failed to set previous error mode")
		}
	}
	catch (...)
	{
		TRACE(L"failed to set previous error mode")
	}
}
