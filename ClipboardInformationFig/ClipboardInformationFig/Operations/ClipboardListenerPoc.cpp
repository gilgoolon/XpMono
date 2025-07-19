#include "BitmapFormat.hpp"
#include "Buffer.hpp"
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <windows.h>

constexpr auto CLASS_NAME = L"ClipboardListenerClass";
constexpr auto WINDOW_NAME = L"Clipboard Listener";
constexpr size_t MAX_HASH_HISTORY = 5;
size_t g_image_hash_history[MAX_HASH_HISTORY] = {};
size_t g_hash_index = 0;

size_t HashMemory(const void* data, const size_t size)
{
	return std::hash<std::string_view>{}(
		std::string_view(static_cast<const char*>(data), size)
	);
}

bool IsDuplicateHash(const size_t current_hash)
{
	for (size_t i = 0; i < MAX_HASH_HISTORY; ++i)
	{
		if (g_image_hash_history[i] == current_hash)
			return true;
	}
	return false;
}

void SaveHash(const size_t current_hash)
{
	g_image_hash_history[g_hash_index] = current_hash;
	g_hash_index = (g_hash_index + 1) % MAX_HASH_HISTORY;
}

std::wstring GetTimestampedFilePath(const std::wstring& folder)
{
	const auto now = std::chrono::system_clock::now();
	const std::time_t raw_time = std::chrono::system_clock::to_time_t(now);

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
	const auto* header = static_cast<BITMAPINFOHEADER*>(dib);
	BYTE* pixel_data = static_cast<BYTE*>(dib) + header->biSize;

	BITMAP bitmap = {};
	bitmap.bmWidth = header->biWidth;
	bitmap.bmHeight = header->biHeight;
	bitmap.bmPlanes = header->biPlanes;
	bitmap.bmBitsPixel = header->biBitCount;
	bitmap.bmBits = pixel_data;

	DWORD image_size = header->biSizeImage;
	if (image_size == 0)
	{
		const int bytes_per_line = ((bitmap.bmWidth * bitmap.bmBitsPixel + 31) / 32) * 4;
		image_size = bytes_per_line * bitmap.bmHeight;
	}

	const Buffer pixel_buffer(pixel_data, pixel_data + image_size);

	Buffer bmp_file = Formats::Bitmap::create(bitmap, pixel_buffer);

	std::ofstream file(path, std::ios::binary);
	if (file.is_open())
	{
		file.write(reinterpret_cast<char*>(bmp_file.data()), bmp_file.size());
	}
}

LRESULT CALLBACK WindowProc(const HWND hwnd, const UINT u_msg, const WPARAM w_param, LPARAM l_param)
{
	switch (u_msg)
	{
	case WM_CLIPBOARDUPDATE:
		if (OpenClipboard(nullptr))
		{
			HANDLE h_data = GetClipboardData(CF_TEXT);
			if (h_data != nullptr && h_data != INVALID_HANDLE_VALUE)
			{
				auto clipboard_text = static_cast<char*>(GlobalLock(h_data));
				if (clipboard_text != nullptr)
				{
					const int len = MultiByteToWideChar(CP_ACP, 0, clipboard_text, -1, nullptr, 0);
					std::wstring wstr_clipboard_text;

					if (len > 0)
					{
						wstr_clipboard_text.resize(len);
						MultiByteToWideChar(CP_ACP, 0, clipboard_text, -1, &wstr_clipboard_text[0], len);
					}

					const std::wstring output = L"[Clipboard Changed] " + wstr_clipboard_text + L"\n";
					OutputDebugStringW(output.c_str());
					GlobalUnlock(clipboard_text);
				}
			}

			h_data = GetClipboardData(CF_DIB);
			if (h_data != nullptr && h_data != INVALID_HANDLE_VALUE)
			{
				void* dib_data = GlobalLock(h_data);

				if (dib_data)
				{
					// Check that the screenshot is not duplicate
					const auto* header = static_cast<BITMAPINFOHEADER*>(dib_data);
					const size_t current_header_hash = HashMemory(header, sizeof(BITMAPINFOHEADER));

					if (!IsDuplicateHash(current_header_hash))
					{
						SaveHash(current_header_hash);

						const std::wstring screenshot_path = GetTimestampedFilePath(L"C:/Users/97254/Desktop/tmp/");
						SaveDIBToFile(dib_data, screenshot_path);

						const std::wstring output = L"[Clipboard Changed] Screenshot saved to path: " + screenshot_path
							+ L"\n";
						OutputDebugStringW(output.c_str());
					}

					GlobalUnlock(h_data);
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
		return DefWindowProc(hwnd, u_msg, w_param, l_param);
	}
	return 0;
}

int WINAPI wWinMain(
	[[maybe_unused]] const HINSTANCE h_instance,
	[[maybe_unused]] const HINSTANCE h_prev_instance,
	[[maybe_unused]] const PWSTR p_cmd_ine,
	[[maybe_unused]] const int n_cmd_show)
{
	HWND hwnd;
	WNDCLASSW wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = h_instance;
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
		h_instance,
		nullptr
	);

	if (!hwnd)
	{
		OutputDebugStringW(L"Failed to create window.\n");
		return EXIT_FAILURE;
	}

	if (!AddClipboardFormatListener(hwnd))
	{
		OutputDebugStringW(L"Failed to add clipboard format listener.\n");
		return 1;
	}

	OutputDebugStringW(L"Listening for clipboard updates...\n\n");

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return EXIT_SUCCESS;
}
