#include "Synchronization/UnmanagedEvent.hpp"

UnmanagedEvent::UnmanagedEvent(const HANDLE handle):
	m_handle(handle)
{
}

HANDLE UnmanagedEvent::handle() const
{
	return m_handle;
}
