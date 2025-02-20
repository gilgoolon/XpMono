#pragma once
#include "DeviceContext.hpp"
#include "Utils/Buffer.hpp"

class ScreenBitmap final
{
public:
	explicit ScreenBitmap(std::shared_ptr<DeviceContext> target_screen);
	~ScreenBitmap();
	ScreenBitmap(const ScreenBitmap&) = delete;
	ScreenBitmap& operator=(const ScreenBitmap&) = delete;
	ScreenBitmap(ScreenBitmap&&) = delete;
	ScreenBitmap& operator=(ScreenBitmap&&) = delete;

private:
	[[nodiscard]] static HBITMAP create_compatible(DeviceContext& device_context);
	[[nodiscard]] Buffer get_bmp_buffer(HBITMAP object) const;

public:
	[[nodiscard]] Buffer capture() const;

private:
	std::shared_ptr<DeviceContext> m_target_screen;
	HBITMAP m_bitmap;
	HGDIOBJ m_previous_bitmap;
};
