#include "Media/ScreenBitmap.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Media/BitmapFormat.hpp"
#include "Media/Dimensions.hpp"

ScreenBitmap::ScreenBitmap(std::shared_ptr<DeviceContext> target_screen):
	m_target_screen(std::move(target_screen)),
	m_bitmap(create_compatible(*m_target_screen)),
	m_previous_bitmap(m_target_screen->select_object(m_bitmap))
{
}

ScreenBitmap::~ScreenBitmap()
{
	try
	{
		m_target_screen->select_object(m_previous_bitmap);
	}
	catch (...)
	{
		TRACE(L"failed to switch back to previous DC bitmap");
	}
}

HBITMAP ScreenBitmap::create_compatible(DeviceContext& device_context)
{
	Dimensions::Dimensions dimensions = Dimensions::get_dimensions();
	const int32_t width = dimensions.right_bound - dimensions.left_bound;
	const int32_t height = dimensions.bottom_bound - dimensions.top_bound;
	const HBITMAP result = CreateCompatibleBitmap(
		device_context.m_dependent_device_context->m_device_context,
		width,
		height
	);
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_BMP_CREATE_COMPATIBLE);
	}
	return result;
}

Buffer ScreenBitmap::get_bmp_buffer([[maybe_unused]] const HBITMAP object) const
{
	if (m_target_screen->m_dependent_device_context == nullptr)
	{
		throw Exception(ErrorCode::FAILED_BMP_INVALID_DC);
	}

	BITMAP bmp;
	if (GetObjectA(object, sizeof(BITMAP), &bmp) == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_BMP_SERIALIZE);
	}

	static constexpr uint32_t BYTES_PER_PIXEL = 4;
	BITMAPINFOHEADER bi_header = Formats::Bitmap::make_info_header(bmp);

	Buffer pixel_data(bmp.bmWidth * bmp.bmHeight * BYTES_PER_PIXEL);

	static constexpr UINT FIRST_LINE = 0;
	const int result = GetDIBits(
		m_target_screen->m_dependent_device_context->m_device_context,
		object,
		FIRST_LINE,
		bmp.bmHeight,
		pixel_data.data(),
		reinterpret_cast<BITMAPINFO*>(&bi_header),
		DIB_RGB_COLORS
	);
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_BMP_SERIALIZE);
	}
	return Formats::Bitmap::create(bmp, pixel_data);
}

Buffer ScreenBitmap::capture() const
{
	if (m_target_screen->m_dependent_device_context == nullptr)
	{
		throw Exception(ErrorCode::FAILED_BMP_INVALID_DC);
	}
	const Dimensions::Dimensions dimensions = Dimensions::get_dimensions();
	const int32_t width = dimensions.right_bound - dimensions.left_bound;
	const int32_t height = dimensions.bottom_bound - dimensions.top_bound;
	static constexpr int32_t LOGICAL_START_BITMAP = 0;
	const BOOL result = BitBlt(
		m_target_screen->m_device_context,
		LOGICAL_START_BITMAP,
		LOGICAL_START_BITMAP,
		width,
		height,
		m_target_screen->m_dependent_device_context->m_device_context,
		dimensions.left_bound,
		dimensions.top_bound,
		SRCCOPY | CAPTUREBLT
	);
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_BMP_CAPTURE);
	}
	return get_bmp_buffer(m_bitmap);
}
