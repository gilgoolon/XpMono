#include "Services/ServiceIterator.hpp"
#include "Exception.hpp"
#include "Trace.hpp"

ServiceIterator::ServiceIterator() :
	m_next_result(nullptr),
	m_next_index(0)
{
	load_services();
	retrieve_first();
}

std::vector<UnopenedService::Ptr> ServiceIterator::get_all()
{
	const ServiceIterator enumerator;
	std::vector<UnopenedService::Ptr> result;

	while (enumerator.has_next())
	{
		result.push_back(enumerator.next());
	}

	return result;
}

UnopenedService::Ptr ServiceIterator::next() const
{
	if (!has_next())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}

	UnopenedService::Ptr result = std::move(m_next_result);
	m_next_result = nullptr;
	return std::move(result);
}

bool ServiceIterator::has_next() const
{
	if (m_next_result != nullptr)
	{
		return true;
	}
	retrieve_next();
	return m_next_result != nullptr;
}

SC_HANDLE ServiceIterator::get_service_manager_handle()
{
	static constexpr LPCWSTR LOCAL_SERVICE_MANAGER = nullptr;
	static constexpr LPCWSTR DEFAULT_SERVICE_MANAGER_DB = nullptr;
	static constexpr DWORD ENUMERATE_SERVICES_ACCESS = SC_MANAGER_ENUMERATE_SERVICE;

	const SC_HANDLE handle = OpenSCManagerW(
		LOCAL_SERVICE_MANAGER,
		DEFAULT_SERVICE_MANAGER_DB,
		ENUMERATE_SERVICES_ACCESS
	);

	if (handle == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_SERVICE_ITERATOR_CREATE);
	}

	return handle;
}

void ServiceIterator::load_services() const
{
	const SC_HANDLE service_manager = get_service_manager_handle();
	DWORD bytes_needed = 0;
	DWORD services_returned = 0;
	DWORD resume_handle = 0;

	static constexpr SC_ENUM_TYPE SERVICE_STATUS_INFO = SC_ENUM_PROCESS_INFO;
	static constexpr DWORD ENUM_WIN32_SERVICES = SERVICE_WIN32;
	static constexpr DWORD ENUM_ACTIVE_AND_INACTIVE_SERVICES = SERVICE_STATE_ALL;
	static constexpr LPBYTE WITHOUT_STORE_INFO = nullptr;
	static constexpr DWORD EMPTY_BUFFER_SIZE = 0;

	if (!EnumServicesStatusExW(
		service_manager,
		SERVICE_STATUS_INFO,
		ENUM_WIN32_SERVICES,
		ENUM_ACTIVE_AND_INACTIVE_SERVICES,
		WITHOUT_STORE_INFO,
		EMPTY_BUFFER_SIZE,
		&bytes_needed,
		&services_returned,
		&resume_handle,
		nullptr
	) && GetLastError() != ERROR_MORE_DATA)
	{
		CloseServiceHandle(service_manager);
		throw WinApiException(ErrorCode::FAILED_SERVICE_ITERATOR_NEXT);
	}

	m_services.resize(bytes_needed / sizeof(ENUM_SERVICE_STATUS_PROCESS));
	if (!EnumServicesStatusExW(
		service_manager,
		SERVICE_STATUS_INFO,
		ENUM_WIN32_SERVICES,
		ENUM_ACTIVE_AND_INACTIVE_SERVICES,
		reinterpret_cast<LPBYTE>(m_services.data()),
		static_cast<uint32_t>(m_services.size() * sizeof(ENUM_SERVICE_STATUS_PROCESS)),
		&bytes_needed,
		&services_returned,
		&resume_handle,
		nullptr
	))
	{
		CloseServiceHandle(service_manager);
		throw WinApiException(ErrorCode::FAILED_SERVICE_ITERATOR_NEXT);
	}
}

void ServiceIterator::retrieve_first() const
{
	if (m_services.empty())
	{
		throw WinApiException(ErrorCode::FAILED_SERVICE_ITERATOR_NEXT);
	}

	m_next_result = std::make_unique<UnopenedService>(m_services[m_next_index].lpServiceName);
}

void ServiceIterator::retrieve_next() const
{
	if (m_next_index + 1 >= m_services.size())
	{
		m_next_result = nullptr;
		return;
	}

	++m_next_index;
	m_next_result = std::make_unique<UnopenedService>(m_services[m_next_index].lpServiceName);
}
