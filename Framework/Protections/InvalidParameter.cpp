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

void Protections::InvalidParameter::handler(const wchar_t* const expression,
                                            const wchar_t* const function,
                                            const wchar_t* const file,
                                            const unsigned int line,
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
