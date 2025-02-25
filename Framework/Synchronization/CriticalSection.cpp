#include "Synchronization/CriticalSection.hpp"

#include "Trace.hpp"

CriticalSection::CriticalSection():
	m_section(create_critical_section())
{
}

CriticalSection::~CriticalSection()
{
	try
	{
		DeleteCriticalSection(&m_section);
	}
	catch (...)
	{
		TRACE(L"failed to delete critical section");
	}
}

CRITICAL_SECTION CriticalSection::create_critical_section()
{
	CRITICAL_SECTION section{};
	InitializeCriticalSection(&section);
	return section;
}

void CriticalSection::Unlocker::operator()(CRITICAL_SECTION* section) const
{
	try
	{
		LeaveCriticalSection(section);
	}
	catch (...)
	{
		TRACE(L"failed unlocking critical section");
	}
}

CriticalSection::Acquired CriticalSection::acquire()
{
	EnterCriticalSection(&m_section);
	return Acquired(&m_section);
}
