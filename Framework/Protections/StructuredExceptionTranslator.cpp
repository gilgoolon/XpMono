#include "Protections/StructuredExceptionTranslator.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

#include <eh.h>

Protections::StructuredExceptionTranslator::StructuredExceptionTranslator():
	m_previous_translator(_set_se_translator(translator))
{
}

Protections::StructuredExceptionTranslator::~StructuredExceptionTranslator()
{
	try
	{
		_set_se_translator(m_previous_translator);
	}
	catch (...)
	{
		TRACE(L"failed to set previous structured exception translator")
	}
}

void Protections::StructuredExceptionTranslator::translator(const unsigned int code,
                                                            [[maybe_unused]] _EXCEPTION_POINTERS* const ep)
{
	TRACE(L"CRITICAL EXCEPTION: code: ", code);
	throw CriticalException();
}
