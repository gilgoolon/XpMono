#pragma once
#include "IOperationHandler.hpp"
#include "Synchronization/CriticalSection.hpp"
#include "Synchronization/Event.hpp"

class SystemInformationHandler final : public IOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = 1;

	explicit SystemInformationHandler(std::unique_ptr<Event> operation_event);
	~SystemInformationHandler() override = default;
	SystemInformationHandler(const SystemInformationHandler&) = delete;
	SystemInformationHandler& operator=(const SystemInformationHandler&) = delete;
	SystemInformationHandler(SystemInformationHandler&&) = delete;
	SystemInformationHandler& operator=(SystemInformationHandler&&) = delete;

	[[nodiscard]] Buffer take(uint32_t max_size) override;

	void run() override;

private:
	std::unique_ptr<Event> m_operation_event;
	Buffer m_result_left;
	CriticalSection m_result_lock;
};
