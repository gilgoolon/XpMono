#pragma once
#include "Handlers/IOperationHandler.hpp"

class SniffTimeHandler final : public IOperationHandler
{
public:
	explicit SniffTimeHandler(Time::Duration duration,
	                          std::shared_ptr<IWaitable> quit_event,
	                          std::unique_ptr<Event> operation_event);
	~SniffTimeHandler() override = default;
	SniffTimeHandler(const SniffTimeHandler&) = delete;
	SniffTimeHandler& operator=(const SniffTimeHandler&) = delete;
	SniffTimeHandler(SniffTimeHandler&&) = delete;
	SniffTimeHandler& operator=(SniffTimeHandler&&) = delete;

	void run() override;

private:
	Time::Duration m_duration;
	std::shared_ptr<IWaitable> m_quit_event;
};
