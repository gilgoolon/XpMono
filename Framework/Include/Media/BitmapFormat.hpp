#pragma once

#include "Utils/Buffer.hpp"

#include <Windows.h>

namespace Formats::Bitmap
{
[[nodiscard]] BITMAPINFOHEADER make_info_header(const BITMAP& bitmap);
[[nodiscard]] Buffer create(const BITMAP& bitmap, const Buffer& pixel_data);
[[nodiscard]] BITMAPFILEHEADER make_file_header(uint32_t file_size);
}
