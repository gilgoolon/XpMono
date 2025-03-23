#include "Processes/ActiveWindowsHook.hpp"

void ActiveWindowsHook::register_hook(WindowsHook::Type type, WindowsHook::Callback callback)
{
	g_active_lock = g_critical_section.acquire();

	g_active_hook = std::make_unique<WindowsHook>(type, std::move(callback));
}

void ActiveWindowsHook::unregister_hook()
{
	try
	{
		g_active_hook.reset();
	}
	catch (...)
	{
		// always release the lock to let new hooks be registered
		g_active_lock.reset();
		throw;
	}
	g_active_lock.reset();
}
