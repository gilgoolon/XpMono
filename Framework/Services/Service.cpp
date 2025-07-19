#include "Services/Service.hpp"

#include "Exception.hpp"

Service::Service(const std::wstring& name) :
	m_handle(open_service(std::move(name)))
{
}

SC_HANDLE Service::handle() const
{
	return m_handle.get();
}

SC_HANDLE Service::open_service(const std::wstring& name)
{
	static constexpr LPCWSTR LOCAL_SERVICE_MANAGER = nullptr;
	static constexpr LPCWSTR DEFAULT_SERVICE_MANAGER_DB = nullptr;
	static constexpr DWORD SERVICE_MANAGER_ALL_ACCESS = SC_MANAGER_ALL_ACCESS;

	const SC_HANDLE service_manager = OpenSCManagerW(
		LOCAL_SERVICE_MANAGER,
		DEFAULT_SERVICE_MANAGER_DB,
		SERVICE_MANAGER_ALL_ACCESS
	);
	if (service_manager == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_SERVICE_OPEN_MANAGER);
	}

	const SC_HANDLE service_handle = OpenServiceW(service_manager, name.c_str(), SERVICE_ALL_ACCESS);
	if (service_handle == nullptr)
	{
		CloseServiceHandle(service_manager);
		throw WinApiException(ErrorCode::FAILED_SERVICE_OPEN, GetLastError());
	}

	CloseServiceHandle(service_manager);
	return service_handle;
}

SERVICE_STATUS_PROCESS Service::query_status() const
{
	static constexpr SC_STATUS_TYPE SERVICE_STATUS_INFO = SC_STATUS_PROCESS_INFO;
	static LPDWORD NO_NEED_FOR_BYTES_NEEDED = nullptr;

	SERVICE_STATUS_PROCESS status{};
	if (!QueryServiceStatusEx(
		m_handle.get(),
		SERVICE_STATUS_INFO,
		reinterpret_cast<LPBYTE>(&status),
		sizeof(status),
		NO_NEED_FOR_BYTES_NEEDED
	))
	{
		throw WinApiException(ErrorCode::FAILED_SERVICE_QUERY_STATUS);
	}
	return status;
}

Service::RunningState Service::get_running_state() const
{
	const SERVICE_STATUS_PROCESS status = query_status();
	switch (status.dwCurrentState)
	{
	case SERVICE_RUNNING:
		return RunningState::RUNNING;
	case SERVICE_STOPPED:
		return RunningState::STOPPED;
	case SERVICE_START_PENDING:
		return RunningState::STARTING;
	case SERVICE_STOP_PENDING:
		return RunningState::STOPPING;
	case SERVICE_PAUSE_PENDING:
		return RunningState::PAUSING;
	case SERVICE_PAUSED:
		return RunningState::PAUSED;
	case SERVICE_CONTINUE_PENDING:
		return RunningState::CONTINUING;
	default:
		throw WinApiException(ErrorCode::FAILED_SERVICE_GET_RUNNING_STATE);
	}
}

uint32_t Service::get_pid() const
{
	const SERVICE_STATUS_PROCESS status = query_status();
	if (status.dwProcessId == 0)
	{
		throw WinApiException(ErrorCode::FAILED_SERVICE_GET_PID);
	}
	return status.dwProcessId;
}

bool Service::is_running_in_system_process() const
{
	const SERVICE_STATUS_PROCESS status = query_status();
	return status.dwServiceFlags == SERVICE_RUNS_IN_SYSTEM_PROCESS;
}

Service::Type Service::get_type() const
{
	const SERVICE_STATUS_PROCESS status = query_status();
	switch (status.dwServiceType)
	{
	case SERVICE_WIN32_OWN_PROCESS:
		return Type::WIN32_OWN_PROCESS;
	case SERVICE_WIN32_SHARE_PROCESS:
		return Type::WIN32_SHARE_PROCESS;
	case SERVICE_KERNEL_DRIVER:
		return Type::KERNEL_DRIVER;
	case SERVICE_FILE_SYSTEM_DRIVER:
		return Type::FILE_SYSTEM_DRIVER;
	default:
		throw WinApiException(ErrorCode::FAILED_SERVICE_GET_TYPE);
	}
}
