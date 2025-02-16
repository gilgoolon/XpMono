#include "Media/ScreenBitmap.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
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
		TRACE(L"failed to switch back to previous DC bitmap")
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
	static constexpr uint32_t BITS_PER_BYTE = 8;

	BITMAPINFOHEADER bi_header{};
	bi_header.biSize = sizeof(BITMAPINFOHEADER);
	bi_header.biWidth = bmp.bmWidth;
	bi_header.biHeight = bmp.bmHeight;
	bi_header.biPlanes = bmp.bmPlanes;
	bi_header.biBitCount = BYTES_PER_PIXEL * BITS_PER_BYTE;
	bi_header.biCompression = BI_RGB;

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

	const int row_stride = bmp.bmWidth * BYTES_PER_PIXEL;
	const int padding_size = (BYTES_PER_PIXEL - (row_stride % BYTES_PER_PIXEL)) % BYTES_PER_PIXEL;
	const int padded_row_size = row_stride + padding_size;
	const int data_size = padded_row_size * bmp.bmHeight;
	const int file_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + data_size;

	std::vector<BYTE> bmp_buffer(file_size);

	static constexpr uint16_t BM_SIGNATURE = 0x4D42;
	BITMAPFILEHEADER bm_file_header = {};
	bm_file_header.bfType = BM_SIGNATURE;
	bm_file_header.bfSize = file_size;
	bm_file_header.bfReserved1 = 0;
	bm_file_header.bfReserved2 = 0;
	bm_file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	std::memcpy(bmp_buffer.data(), &bm_file_header, sizeof(BITMAPFILEHEADER));
	std::memcpy(bmp_buffer.data() + sizeof(BITMAPFILEHEADER), &bi_header, sizeof(BITMAPINFOHEADER));
	BYTE* data_ptr = bmp_buffer.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	for (int y = 0; y < bmp.bmHeight; y++)
	{
		const int src_index = y * row_stride;
		std::memcpy(data_ptr, pixel_data.data() + src_index, row_stride);
		data_ptr += row_stride;

		if (padding_size > 0)
		{
			static constexpr uint8_t PADDING = 0;
			std::memset(data_ptr, PADDING, padding_size);
			data_ptr += padding_size;
		}
	}
	return bmp_buffer;
}

Buffer ScreenBitmap::capture() const
{
	if (m_target_screen->m_dependent_device_context == nullptr)
	{
		throw Exception(ErrorCode::FAILED_BMP_INVALID_DC);
	}
	Dimensions::Dimensions dimensions = Dimensions::get_dimensions();
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
