#include "Protections/PureCall.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

Protections::PureCall::PureCall():
	m_previous_handler(_set_purecall_handler(handler))
{
}

Protections::PureCall::~PureCall()
{
	try
	{
		_set_purecall_handler(m_previous_handler);
	}
	catch (...)
	{
		TRACE(L"failed to set previous purecall handler");
	}
}

void Protections::PureCall::handler()
{
	TRACE(L"CRITICAL ERROR: CRT Pure Call");
	throw CriticalException();
}
