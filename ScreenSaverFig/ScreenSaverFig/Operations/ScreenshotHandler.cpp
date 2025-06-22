#include "ScreenshotHandler.hpp"

#include "Media/DeviceContext.hpp"
#include "Media/ScreenBitmap.hpp"
#include "Products/ImageTypedProduct.hpp"

void ScreenshotHandler::run()
{
	const auto virtual_screen = std::make_shared<DeviceContext>();
	const auto target_screen = std::make_shared<DeviceContext>(virtual_screen);
	const ScreenBitmap bitmap(target_screen);
	append(std::make_unique<ImageTypedProduct>(bitmap.capture()));
	finished();
}
