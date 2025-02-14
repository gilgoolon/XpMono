#include "Protections/InvalidParameter.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

Protections::InvalidParameter::InvalidParameter():
	m_prev_handler(_set_invalid_parameter_handler(handler))
{
}

Protections::InvalidParameter::~InvalidParameter()
{
	try
	{
		_set_invalid_parameter_handler(m_prev_handler);
	}
	catch (...)
	{
		TRACE(L"failed to revert invalid parameter handler")
	}
}

void Protections::InvalidParameter::handler([[maybe_unused]] const wchar_t* const expression,
                                            [[maybe_unused]] const wchar_t* const function,
                                            [[maybe_unused]] const wchar_t* const file,
                                            [[maybe_unused]] const unsigned int line,
                                            [[maybe_unused]] const uintptr_t reserved)
{
	TRACE(
		L"CRITICAL ERROR: CRT Invalid Parameter error:\n\tlocation: ",
		file,
		L":",
		line,
		L" ",
		function,
		L"\n\texpression: ",
		expression
	)
	throw CriticalException();
}
