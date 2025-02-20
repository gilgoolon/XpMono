#pragma once
#include "Synchronization/CriticalSection.hpp"

class DeviceContext final
{
public:
	explicit DeviceContext();
	explicit DeviceContext(std::shared_ptr<DeviceContext> device_context);
	~DeviceContext();
	DeviceContext(const DeviceContext& device_context) = delete;
	DeviceContext& operator=(const DeviceContext&) = delete;
	DeviceContext(DeviceContext&&) = delete;
	DeviceContext& operator=(DeviceContext&&) = delete;

private:
	std::shared_ptr<DeviceContext> m_dependent_device_context;
	HDC m_device_context;
	const bool m_existing;

	[[nodiscard]] static HDC get_virtual_screen();
	[[nodiscard]] static HDC create_compatible(const DeviceContext& device_context);

	HGDIOBJ select_object(HGDIOBJ object);

	friend class ScreenBitmap;
};
