#pragma once
#include "FigApi.hpp"
#include "Synchronization/CriticalSection.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Buffer.hpp"

class IOperationHandler
{
public:
	explicit IOperationHandler(std::unique_ptr<Event> operation_event);
	virtual ~IOperationHandler() = default;
	IOperationHandler(const IOperationHandler&) = delete;
	IOperationHandler& operator=(const IOperationHandler&) = delete;
	IOperationHandler(IOperationHandler&&) = delete;
	IOperationHandler& operator=(IOperationHandler&&) = delete;

	[[nodiscard]] virtual Buffer take(uint32_t max_size);
	[[nodiscard]] virtual Fig::ExecutionStatus status();
	[[nodiscard]] virtual Fig::FigSpecificCode specific_code();

	void error(Fig::FigSpecificCode specific_code);
	void finished();
	void append(const Buffer& data);

	virtual void run() = 0;

private:
	CriticalSection m_lock;
	std::unique_ptr<Event> m_event;
	Buffer m_result;
	Fig::ExecutionStatus m_status;
	Fig::FigSpecificCode m_specific_code;
};
