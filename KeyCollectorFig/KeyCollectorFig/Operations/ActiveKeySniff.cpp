#include "ActiveKeySniff.hpp"

#include "Processes/ActiveWindowsHook.hpp"

void ActiveKeySniff::start()
{
	g_keyboard_events.clear();

	ActiveWindowsHook::register_hook(WindowsHook::Type::KEYBOARD, callback);
}

void ActiveKeySniff::stop()
{
	ActiveWindowsHook::unregister_hook();
}

void ActiveKeySniff::callback(const int action_type, const WPARAM message_type, const LPARAM message_data)
{
	static constexpr int KEYBOARD_ACTION = 0;
	if (action_type != KEYBOARD_ACTION)
	{
		return;
	}

	const auto data = reinterpret_cast<const KBDLLHOOKSTRUCT*>(message_data);

	const uint32_t key_code = data->vkCode;
	const KeyboardEvent::Type event_type = (message_type == WM_KEYDOWN || message_type == WM_SYSKEYDOWN)
		                                       ? KeyboardEvent::Type::DOWN
		                                       : KeyboardEvent::Type::UP;

	g_keyboard_events.push_back(KeyboardEvent{.type = event_type, .key_code = key_code});
}
