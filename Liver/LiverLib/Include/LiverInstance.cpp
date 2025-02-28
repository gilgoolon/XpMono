#include "LiverInstance.hpp"

#include "Synchronization/Event.hpp"

static constexpr auto MUTEX_NAME = L"LiverMutex";

LiverInstance::LiverInstance():
	m_mutex(Event::GLOBAL_NAMESPACE + std::wstring{MUTEX_NAME})
{
}
