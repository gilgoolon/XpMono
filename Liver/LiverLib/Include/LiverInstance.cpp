#include "LiverInstance.hpp"

static constexpr std::wstring_view MUTEX_NAME = L"LiverMutex";

LiverInstance::LiverInstance():
	m_mutex(std::wstring{MUTEX_NAME})
{
}
