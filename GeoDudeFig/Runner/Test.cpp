#include <fstream>
#include <iostream>
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mfreadwrite.h>
#include <shlwapi.h>
#include <windows.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "shlwapi.lib")

void SaveBMP(const BYTE* pData, UINT32 width, UINT32 height, const char* filename)
{
	BITMAPFILEHEADER bfh = {};
	BITMAPINFOHEADER bih = {};

	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = width;
	bih.biHeight = -static_cast<int>(height); // negative for top-down
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;

	DWORD imageSize = width * height * 4;
	bfh.bfType = 0x4D42; // 'BM'
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bfh.bfSize = bfh.bfOffBits + imageSize;

	std::ofstream file(filename, std::ios::binary);
	file.write((char*)&bfh, sizeof(bfh));
	file.write((char*)&bih, sizeof(bih));
	file.write((char*)pData, imageSize);
	file.close();
}

int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance,
                    [[maybe_unused]] HINSTANCE hPrevInstance,
                    [[maybe_unused]] PWSTR pCmdLine,
                    [[maybe_unused]] int nCmdShow)
{
	HRESULT hr = MFStartup(MF_VERSION);
	if (FAILED(hr)) return -1;

	IMFAttributes* pAttributes = nullptr;
	IMFActivate** ppDevices = nullptr;
	UINT32 count = 0;

	hr = MFCreateAttributes(&pAttributes, 1);
	if (FAILED(hr)) return -1;

	hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	if (FAILED(hr)) return -1;

	hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
	if (FAILED(hr) || count == 0) return -1;

	IMFMediaSource* pSource = nullptr;
	hr = ppDevices[0]->ActivateObject(IID_PPV_ARGS(&pSource));
	if (FAILED(hr)) return -1;

	IMFSourceReader* pReader = nullptr;
	hr = MFCreateSourceReaderFromMediaSource(pSource, nullptr, &pReader);
	if (FAILED(hr)) return -1;

	IMFMediaType* pType = nullptr;
	MFCreateMediaType(&pType);
	pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
	pReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), nullptr, pType);

	// Get frame size
	UINT32 width = 0, height = 0;
	MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
	std::wcout << L"Capturing frame: " << width << L"x" << height << std::endl;

	IMFSample* pSample = nullptr;
	DWORD streamIndex, flags;
	LONGLONG timestamp;

	for (int tries = 0; tries < 10 && !pSample; tries++)
	{
		hr = pReader->ReadSample(
			static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM),
			0,
			&streamIndex,
			&flags,
			&timestamp,
			&pSample
		);
	}

	if (!pSample)
	{
		std::cerr << "Failed to capture frame." << std::endl;
		return -1;
	}

	IMFMediaBuffer* pBuffer = nullptr;
	pSample->ConvertToContiguousBuffer(&pBuffer);

	BYTE* pData = nullptr;
	DWORD maxLen = 0, curLen = 0;
	pBuffer->Lock(&pData, &maxLen, &curLen);

	SaveBMP(pData, width, height, "capture.bmp");
	std::cout << "Saved capture.bmp" << std::endl;

	pBuffer->Unlock();

	// Cleanup
	pBuffer->Release();
	pSample->Release();
	pType->Release();
	pReader->Release();
	pSource->Release();
	for (UINT32 i = 0; i < count; i++) ppDevices[i]->Release();
	CoTaskMemFree(ppDevices);
	pAttributes->Release();
	MFShutdown();

	return 0;
}
