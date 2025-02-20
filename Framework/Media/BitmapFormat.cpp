#include "Media/BitmapFormat.hpp"

#include <cstdint>

BITMAPINFOHEADER Formats::Bitmap::make_info_header(const BITMAP& bitmap)
{
	static constexpr uint8_t BITS_PER_PIXEL = 32;
	BITMAPINFOHEADER result{};
	result.biSize = sizeof(BITMAPINFOHEADER);
	result.biWidth = bitmap.bmWidth;
	result.biHeight = bitmap.bmHeight;
	result.biPlanes = bitmap.bmPlanes;
	result.biBitCount = BITS_PER_PIXEL;
	result.biCompression = BI_RGB;
	return result;
}

Buffer Formats::Bitmap::create(const BITMAP& bitmap, const Buffer& pixel_data)
{
	static constexpr uint16_t BYTES_PER_PIXEL = 4;
	const uint32_t row_stride = bitmap.bmWidth * BYTES_PER_PIXEL;
	const uint32_t padding_size = (BYTES_PER_PIXEL - (row_stride % BYTES_PER_PIXEL)) % BYTES_PER_PIXEL;
	const uint32_t padded_row_size = row_stride + padding_size;
	const uint32_t data_size = padded_row_size * bitmap.bmHeight;
	const uint32_t file_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + data_size;

	const BITMAPINFOHEADER bi_header = make_info_header(bitmap);
	const BITMAPFILEHEADER bm_file_header = make_file_header(file_size);

	Buffer result(file_size);

	std::memcpy(result.data(), &bm_file_header, sizeof(BITMAPFILEHEADER));
	std::memcpy(result.data() + sizeof(BITMAPFILEHEADER), &bi_header, sizeof(BITMAPINFOHEADER));
	BYTE* data_ptr = result.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	for (uint32_t y = 0; y < static_cast<uint32_t>(bitmap.bmHeight); y++)
	{
		const uint32_t src_index = y * row_stride;
		std::memcpy(data_ptr, pixel_data.data() + src_index, row_stride);
		data_ptr += row_stride;

		if (padding_size > 0)
		{
			static constexpr uint8_t PADDING = 0;
			std::memset(data_ptr, PADDING, padding_size);
			data_ptr += padding_size;
		}
	}
	return result;
}

BITMAPFILEHEADER Formats::Bitmap::make_file_header(const uint32_t file_size)
{
	static constexpr uint16_t BM_SIGNATURE = 0x4D42;
	BITMAPFILEHEADER result = {};
	result.bfType = BM_SIGNATURE;
	result.bfSize = file_size;
	result.bfReserved1 = 0;
	result.bfReserved2 = 0;
	result.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	return result;
}
