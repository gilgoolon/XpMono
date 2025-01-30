#pragma once
#include "IWaitable.hpp"
#include "Processes/ScopedHandle.hpp"

class Event final : public IWaitable
{
public:
	enum class Type : uint32_t
	{
		MANUAL_RESET = FALSE,
		AUTO_RESET = TRUE,
	};

	explicit Event(const std::wstring& name);
	explicit Event(const std::wstring& name, Type type);
	~Event() override = default;
	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;
	Event(Event&&) = delete;
	Event& operator=(Event&&) = delete;

private:
	[[nodiscard]] HANDLE handle() const override;

	[[nodiscard]] static HANDLE open_event(const std::wstring& name);

	[[nodiscard]] static HANDLE create_event(const std::wstring& name, Type type);

	ScopedHandle m_handle;
};
