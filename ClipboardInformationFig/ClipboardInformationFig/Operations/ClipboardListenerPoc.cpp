#include "Media/BitmapFormat.hpp"
#include "Utils/Buffer.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <windows.h>

#include <iomanip>
#include <sstream>
#include <string>

HWND hwnd;

std::wstring GetTimestampedFilePath(const std::wstring& folder)
{
	auto now = std::chrono::system_clock::now();
	std::time_t raw_time = std::chrono::system_clock::to_time_t(now);

	struct tm time_info;
	if (localtime_s(&time_info, &raw_time) != 0)
	{
		return folder + L"clipboard_unknown_time.bmp";
	}

	std::wstringstream ss;
	ss << folder
		<< L"clipboard_"
		<< std::put_time(&time_info, L"%Y-%m-%d_%H-%M-%S")
		<< L".bmp";

	return ss.str();
}

void SaveDIBToFile(void* dib, const std::wstring& path)
{
	auto* header = reinterpret_cast<BITMAPINFOHEADER*>(dib);
	BYTE* pixel_data = reinterpret_cast<BYTE*>(dib) + header->biSize;

	BITMAP bitmap = {};
	bitmap.bmWidth = header->biWidth;
	bitmap.bmHeight = header->biHeight;
	bitmap.bmPlanes = header->biPlanes;
	bitmap.bmBitsPixel = header->biBitCount;
	bitmap.bmBits = pixel_data;

	// Calculate image size (if not set)
	DWORD imageSize = header->biSizeImage;
	if (imageSize == 0)
	{
		int bytesPerLine = ((bitmap.bmWidth * bitmap.bmBitsPixel + 31) / 32) * 4;
		imageSize = bytesPerLine * bitmap.bmHeight;
	}

	Buffer pixelBuffer(pixel_data, pixel_data + imageSize);

	Buffer bmpFile = Formats::Bitmap::create(bitmap, pixelBuffer);

	// Save to file
	std::ofstream file(path, std::ios::binary);
	if (file.is_open())
	{
		file.write(reinterpret_cast<char*>(bmpFile.data()), bmpFile.size());
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLIPBOARDUPDATE:
		if (OpenClipboard(nullptr))
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			if (hData != nullptr && hData != INVALID_HANDLE_VALUE)
			{
				auto pszText = static_cast<char*>(GlobalLock(hData));
				if (pszText != nullptr)
				{
					// Convert to wide string
					int len = MultiByteToWideChar(CP_ACP, 0, pszText, -1, nullptr, 0);
					std::wstring wstrpszText;

					if (len > 0)
					{
						wstrpszText.resize(len);
						MultiByteToWideChar(CP_ACP, 0, pszText, -1, &wstrpszText[0], len);
					}

					std::wstring wstrText = L"\n[Clipboard Changed] " + wstrpszText;
					OutputDebugStringW(wstrText.c_str());
					GlobalUnlock(hData);
				}
			}

			hData = GetClipboardData(CF_DIB);
			if (hData != nullptr && hData != INVALID_HANDLE_VALUE)
			{
				void* dibData = GlobalLock(hData);

				if (dibData)
				{
					const std::wstring screenshot_path = GetTimestampedFilePath(L"C:/Users/97254/Desktop/tmp/");
					SaveDIBToFile(dibData, screenshot_path);
					GlobalUnlock(hData);
					const std::wstring wstrText = L"\n[Clipboard Changed] Screenshot saved to path: " + screenshot_path;
					OutputDebugStringW(wstrText.c_str());
				}
			}
			CloseClipboard();
		}
		break;

	case WM_DESTROY:
		RemoveClipboardFormatListener(hwnd);
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(
	[[maybe_unused]] HINSTANCE hInstance,
	[[maybe_unused]] HINSTANCE hPrevInstance,
	[[maybe_unused]] PWSTR pCmdLine,
	[[maybe_unused]] int nCmdShow)
{
	const auto CLASS_NAME = L"ClipboardListenerClass";
	const auto WINDOW_NAME = L"Clipboard Listener";

	WNDCLASSW wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClassW(&wc);

	hwnd = CreateWindowExW(
		0,
		CLASS_NAME,
		WINDOW_NAME,
		0,
		0,
		0,
		0,
		0,
		HWND_MESSAGE,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hwnd)
	{
		OutputDebugStringW(L"Failed to create window.");
		return EXIT_FAILURE;
	}

	// Register clipboard listener
	if (!AddClipboardFormatListener(hwnd))
	{
		OutputDebugStringW(L"Failed to add clipboard format listener.");
		return 1;
	}

	OutputDebugStringW(L"Listening for clipboard updates...");

	// Message loop
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return EXIT_SUCCESS;
}
