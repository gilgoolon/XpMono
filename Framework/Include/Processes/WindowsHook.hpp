#pragma once

#include "Synchronization/CriticalSection.hpp"

#include <functional>
#include <Windows.h>

class WindowsHook final
{
public:
	enum class Type : int
	{
		KEYBOARD = WH_KEYBOARD_LL,
	};

	using Callback = std::function<void(int)>;

	explicit WindowsHook(Type type, Callback callback);
	~WindowsHook();
	WindowsHook(const WindowsHook&) = delete;
	WindowsHook& operator=(const WindowsHook&) = delete;
	WindowsHook(WindowsHook&&) = delete;
	WindowsHook& operator=(WindowsHook&&) = delete;

private:
	CriticalSection::Acquired m_lock;
	Callback m_callback;
	HHOOK m_hook;

	[[nodiscard]] HHOOK register_hook(Type type);

	static LRESULT CALLBACK hook(int code, WPARAM w_param, LPARAM l_param);
};
