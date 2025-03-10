#pragma once
#include "Utils/Time.hpp"

#include <cstdint>
#include <optional>

enum class WaitStatus : uint32_t
{
	FINISHED = 0,
	OBJECT_CLOSED,
	TIMEOUT,
	FAILED,
};

struct WaitResult final
{
	WaitStatus status;
	std::optional<uint32_t> triggered_object;
};

class IWaitable
{
public:
	explicit IWaitable() = default;
	virtual ~IWaitable() = default;
	IWaitable(const IWaitable&) = delete;
	IWaitable& operator=(const IWaitable&) = delete;
	IWaitable(IWaitable&&) = delete;
	IWaitable& operator=(IWaitable&&) = delete;

	[[nodiscard]] virtual HANDLE handle() const = 0;

	[[nodiscard]] WaitStatus wait(Time::Duration timeout) const;

	static void sleep(Time::Duration duration);

	[[nodiscard]] static WaitResult wait_for_any(const std::vector<std::shared_ptr<IWaitable>>& objects,
	                                             Time::Duration timeout);

	[[nodiscard]] static WaitResult wait_for_all(const std::vector<std::shared_ptr<IWaitable>>& objects,
	                                             Time::Duration timeout);

private:
	[[nodiscard]] static WaitResult wait_for_multiple(const std::vector<std::shared_ptr<IWaitable>>& objects,
	                                                  Time::Duration timeout,
	                                                  bool wait_all);
};
