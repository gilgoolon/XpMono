#include "LiverInstance.hpp"

#include "Synchronization/Event.hpp"

static constexpr auto MUTEX_NAME =
#ifdef _DEBUG
	L"LiverMutex";
#else
	L"{95F97CCC-4DCD-4D38-8744-F31636E20A62}"
#endif

LiverInstance::LiverInstance():
	m_mutex(Event::GLOBAL_NAMESPACE + std::wstring{MUTEX_NAME})
{
}
