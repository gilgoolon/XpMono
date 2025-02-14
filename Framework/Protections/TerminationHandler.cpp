#include "Protections/TerminationHandler.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

TerminationHandler::TerminationHandler():
	m_previous_handler(set_terminate(handler))
{
}

TerminationHandler::~TerminationHandler()
{
	try
	{
		set_terminate(m_previous_handler);
	}
	catch (...)
	{
		TRACE(L"failed to set previous termination handler")
	}
}

void TerminationHandler::handler()
{
	TRACE(L"CRITICAL ERROR: termination handler called")
	throw CriticalException();
}
