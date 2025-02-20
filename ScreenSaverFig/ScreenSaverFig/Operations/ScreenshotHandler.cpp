#include "ScreenshotHandler.hpp"

#include "Media/DeviceContext.hpp"
#include "Media/ScreenBitmap.hpp"

void ScreenshotHandler::run()
{
	const auto virtual_screen = std::make_shared<DeviceContext>();
	const auto target_screen = std::make_shared<DeviceContext>(virtual_screen);
	const ScreenBitmap bitmap(target_screen);
	append(bitmap.capture());
	finished();
}
