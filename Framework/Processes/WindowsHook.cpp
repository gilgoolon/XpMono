#include "Processes/WindowsHook.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Processes/ActiveWindowsHook.hpp"

WindowsHook::WindowsHook(const Type type, Callback callback) :
	m_callback(std::move(callback)),
	m_hook(register_hook(type))
{
}

WindowsHook::~WindowsHook()
{
	try
	{
		if (UnhookWindowsHookEx(m_hook) == FALSE)
		{
			TRACE(L"failed to unhook WindowsHook hook");
		}
	}
	catch (...)
	{
		TRACE(L"failed to unhook WindowsHook hook");
	}
}

HHOOK WindowsHook::register_hook(Type type)
{
	const HHOOK result = SetWindowsHookW(static_cast<int>(type), hook);
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_WINDOWS_HOOK_REGISTER);
	}
	return result;
}

LRESULT WindowsHook::hook(const int code, const WPARAM w_param, const LPARAM l_param)
{
	if (ActiveWindowsHook::g_active_hook != nullptr)
	{
		try
		{
			ActiveWindowsHook::g_active_hook->m_callback(code);
		}
		catch (...)
		{
			TRACE(L"failed to call WindowsHook callback");
		}
	}

	static constexpr HHOOK IGNORED = nullptr;
	return CallNextHookEx(IGNORED, code, w_param, l_param);
}
