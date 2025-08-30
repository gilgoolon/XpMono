#pragma once

#include "ScopedServiceHandler.hpp"
#include "Media/MediaFoundation/Attributes.hpp"

#include <memory>
#include <string>

class Service final : public std::enable_shared_from_this<Service>
{
public:
	using Ptr = std::unique_ptr<Service>;

	enum class RunningState
	{
		RUNNING,
		STOPPED,
		STARTING,
		STOPPING,
		PAUSING,
		PAUSED,
		CONTINUING
	};

	enum class Type
	{
		WIN32_OWN_PROCESS,
		WIN32_SHARE_PROCESS,
		KERNEL_DRIVER,
		FILE_SYSTEM_DRIVER
	};

	explicit Service(const std::wstring& name);
	~Service() = default;
	Service(const Service&) = delete;
	Service& operator=(const Service&) = delete;
	Service(Service&&) = delete;
	Service& operator=(Service&&) = delete;

	[[nodiscard]] RunningState get_running_state() const;

	[[nodiscard]] uint32_t get_pid() const;

	[[nodiscard]] bool is_running_in_system_process() const;

	[[nodiscard]] Type get_type() const;

private:
	[[nodiscard]] SC_HANDLE handle() const;

	[[nodiscard]] static SC_HANDLE open_service(const std::wstring& name);

	[[nodiscard]] SERVICE_STATUS_PROCESS query_status() const;

	ScopedServiceHandle m_handle;
};
