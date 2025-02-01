#pragma once
#include "IWaitable.hpp"

#include <Windows.h>

class UnmanagedEvent final : public IWaitable
{
public:
	explicit UnmanagedEvent(HANDLE handle);
	~UnmanagedEvent() override = default;
	UnmanagedEvent(const UnmanagedEvent&) = delete;
	UnmanagedEvent& operator=(const UnmanagedEvent&) = delete;
	UnmanagedEvent(UnmanagedEvent&&) = delete;
	UnmanagedEvent& operator=(UnmanagedEvent&&) = delete;

	HANDLE m_handle;

	[[nodiscard]] HANDLE handle() const override;
};
