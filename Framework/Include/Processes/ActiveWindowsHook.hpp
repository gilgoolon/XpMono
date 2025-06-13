#pragma once
#include "WindowsHook.hpp"

class ActiveWindowsHook final
{
public:
	explicit ActiveWindowsHook() = delete;
	~ActiveWindowsHook() = delete;
	ActiveWindowsHook(const ActiveWindowsHook&) = delete;
	ActiveWindowsHook& operator=(const ActiveWindowsHook&) = delete;
	ActiveWindowsHook(ActiveWindowsHook&&) = delete;
	ActiveWindowsHook& operator=(ActiveWindowsHook&&) = delete;

	friend WindowsHook;

	static void register_hook(WindowsHook::Type type, WindowsHook::Callback callback);
	static void unregister_hook();

private:
	static std::unique_ptr<WindowsHook> g_active_hook;
	static CriticalSection g_critical_section;
	static CriticalSection::Acquired g_active_lock;
};
