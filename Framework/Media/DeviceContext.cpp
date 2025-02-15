#include "Media/DeviceContext.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

DeviceContext::DeviceContext():
	m_device_context(get_virtual_screen()),
	m_existing(false)
{
}

DeviceContext::DeviceContext(const DeviceContext& device_context):
	m_device_context(create_compatible(device_context)),
	m_existing(true)
{
}

DeviceContext::~DeviceContext()
{
	try
	{
		if (!m_existing)
		{
			static constexpr HWND VIRTUAL_SCREEN = nullptr;
			if (ReleaseDC(VIRTUAL_SCREEN, m_device_context) == FALSE)
			{
				TRACE(L"failed to release DC")
			}
		}
		else
		{
			if (DeleteDC(m_device_context) == FALSE)
			{
				TRACE(L"failed to delete DC")
			}
		}
	}
	catch (...)
	{
		TRACE(L"failed to get destroy display context")
	}
}

HDC DeviceContext::get_virtual_screen()
{
	static constexpr HWND VIRTUAL_SCREEN = nullptr;
	const HDC result = GetDC(VIRTUAL_SCREEN);
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_DC_GET);
	}
	return result;
}

HDC DeviceContext::create_compatible(const DeviceContext& device_context)
{
	const HDC result = CreateCompatibleDC(device_context.m_device_context);
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_DC_CREATE_COMPATIBLE);
	}
	return result;
}
