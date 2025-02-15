#include "Media/ScreenBitmap.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Media/Dimensions.hpp"

ScreenBitmap::ScreenBitmap(std::shared_ptr<DeviceContext> device_context):
	m_device_context(std::move(device_context)),
	m_bitmap(create_compatible(*m_device_context)),
	m_previous_bitmap(m_device_context->select_object(m_bitmap))
{
}

ScreenBitmap::~ScreenBitmap()
{
	try
	{
		m_device_context->select_object(m_previous_bitmap);
	}
	catch (...)
	{
		TRACE(L"failed to switch back to previous DC bitmap")
	}
}

HBITMAP ScreenBitmap::create_compatible(DeviceContext& device_context)
{
	Dimensions::Dimensions dimensions = Dimensions::get_dimensions();
	const int32_t width = dimensions.right_bound - dimensions.left_bound;
	const int32_t height = dimensions.bottom_bound - dimensions.top_bound;
	const HBITMAP result = CreateCompatibleBitmap(device_context.m_device_context, width, height);
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_BMP_CREATE_COMPATIBLE);
	}
	return result;
}

Buffer ScreenBitmap::capture() const
{
	return {};
}
