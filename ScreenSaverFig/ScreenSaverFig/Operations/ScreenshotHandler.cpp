#include "ScreenshotHandler.hpp"

#include <gdiplus.h>
#include <Windows.h>

static Buffer make_bmp(HBITMAP hBitmap, const Buffer& pixelData)
{
	BITMAP bmp;
	GetObject(hBitmap, sizeof(BITMAP), &bmp);

	if (pixelData.empty())
		return {};

	// BMP format requires padding for rows to be a multiple of 4 bytes
	int bytesPerPixel = 4; // 32-bit (BGRA)
	int rowStride = bmp.bmWidth * bytesPerPixel;
	int paddingSize = (4 - (rowStride % 4)) % 4;
	int paddedRowSize = rowStride + paddingSize;
	int dataSize = paddedRowSize * bmp.bmHeight;
	int fileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dataSize;

	// Allocate buffer for the BMP file
	std::vector<BYTE> bmpBuffer(fileSize);

	// BMP file header
	BITMAPFILEHEADER bmfHeader = {};
	bmfHeader.bfType = 0x4D42; // 'BM'
	bmfHeader.bfSize = fileSize;
	bmfHeader.bfReserved1 = 0;
	bmfHeader.bfReserved2 = 0;
	bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// BMP info header
	BITMAPINFOHEADER biHeader = {};
	biHeader.biSize = sizeof(BITMAPINFOHEADER);
	biHeader.biWidth = bmp.bmWidth;
	biHeader.biHeight = bmp.bmHeight; // Positive height for bottom-up DIB
	biHeader.biPlanes = 1;
	biHeader.biBitCount = 32; // BGRA format
	biHeader.biCompression = BI_RGB;
	biHeader.biSizeImage = dataSize;

	// Copy headers to the buffer
	std::memcpy(bmpBuffer.data(), &bmfHeader, sizeof(BITMAPFILEHEADER));
	std::memcpy(bmpBuffer.data() + sizeof(BITMAPFILEHEADER), &biHeader, sizeof(BITMAPINFOHEADER));

	// Copy pixel data row by row (BMP is bottom-up)
	BYTE* dataPtr = bmpBuffer.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	for (int y = 0; y < bmp.bmHeight; y++)
	{
		int srcIndex = (bmp.bmHeight - 1 - y) * rowStride; // Bottom-up BMP
		std::memcpy(dataPtr, &pixelData[srcIndex], rowStride);
		dataPtr += rowStride;

		// Add padding if needed
		if (paddingSize > 0)
		{
			std::memset(dataPtr, 0, paddingSize);
			dataPtr += paddingSize;
		}
	}

	return bmpBuffer;
}

static Buffer serialize_bitmap(HBITMAP hBitmap)
{
	if (!hBitmap)
		return {};

	BITMAP bmp;
	if (!GetObject(hBitmap, sizeof(BITMAP), &bmp))
		return {};

	BITMAPINFOHEADER bi = {0};
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmp.bmWidth;
	bi.biHeight = -bmp.bmHeight; // Negative to keep the image top-down
	bi.biPlanes = 1;
	bi.biBitCount = 32; // Ensure we get 32-bit BGRA pixels
	bi.biCompression = BI_RGB;

	std::vector<BYTE> pixelData(bmp.bmWidth * bmp.bmHeight * 4); // 4 bytes per pixel (BGRA)

	HDC hDC = GetDC(nullptr);
	if (!hDC)
		return {};

	if (!GetDIBits(
		hDC,
		hBitmap,
		0,
		bmp.bmHeight,
		pixelData.data(),
		reinterpret_cast<BITMAPINFO*>(&bi),
		DIB_RGB_COLORS
	))
	{
		pixelData.clear();
	}

	ReleaseDC(nullptr, hDC);
	return pixelData;
}

void ScreenshotHandler::run()
{
	int x = GetSystemMetrics(SM_XVIRTUALSCREEN); //left (e.g. -1024)
	int y = GetSystemMetrics(SM_YVIRTUALSCREEN); //top (e.g. -34)
	int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN); //entire width (e.g. 2704)
	int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN); //entire height (e.g. 1050)

	HDC dcScreen = GetDC(nullptr);
	HDC dcTarget = CreateCompatibleDC(dcScreen);
	HBITMAP bmpTarget = CreateCompatibleBitmap(dcScreen, cx, cy);
	HGDIOBJ oldBmp = SelectObject(dcTarget, bmpTarget);
	BitBlt(dcTarget, 0, 0, cx, cy, dcScreen, x, y, SRCCOPY | CAPTUREBLT);
	append(make_bmp(bmpTarget, serialize_bitmap(bmpTarget)));
	SelectObject(dcTarget, oldBmp);
	DeleteDC(dcTarget);
	ReleaseDC(nullptr, dcScreen);
	finished();
}
