#pragma once
#include "IOperationHandler.hpp"
#include "Synchronization/Event.hpp"

class DirlistHandler final : public IOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = 1;

	explicit DirlistHandler(std::unique_ptr<Event> operation_event);
	~DirlistHandler() override = default;
	DirlistHandler(const DirlistHandler&) = delete;
	DirlistHandler& operator=(const DirlistHandler&) = delete;
	DirlistHandler(DirlistHandler&&) = delete;
	DirlistHandler& operator=(DirlistHandler&&) = delete;

	[[nodiscard]] Buffer take(uint32_t max_size) override;
	[[nodiscard]] Fig::ExecutionStatus status() override;
	[[nodiscard]] Fig::FigSpecificCode specific_code() override;

	void run() override;

private:
	std::unique_ptr<Event> m_operation_event;
	Fig::ExecutionStatus m_status;
	Fig::FigSpecificCode m_code;
	Buffer m_data; // TODO: replace with actual logic
};
