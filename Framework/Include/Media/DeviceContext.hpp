#pragma once
#include "Synchronization/CriticalSection.hpp"

class DeviceContext final
{
public:
	explicit DeviceContext();
	explicit DeviceContext(const DeviceContext& device_context);
	~DeviceContext();
	DeviceContext& operator=(const DeviceContext&) = delete;
	DeviceContext(DeviceContext&&) = delete;
	DeviceContext& operator=(DeviceContext&&) = delete;

private:
	HDC m_device_context;
	const bool m_existing;

	[[nodiscard]] static HDC get_virtual_screen();
	[[nodiscard]] static HDC create_compatible(const DeviceContext& device_context);
};
