#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"
#include "Synchronization/Event.hpp"

class TakeWebcamPictureHandler final : public IOperationHandler
{
public:
	explicit TakeWebcamPictureHandler(std::unique_ptr<Event> operation_event, uint32_t device_index);
	explicit TakeWebcamPictureHandler(std::unique_ptr<Event> operation_event, const Json& parameters);
	explicit TakeWebcamPictureHandler(std::unique_ptr<Event> operation_event, const Buffer& raw_parameters);
	~TakeWebcamPictureHandler() override = default;
	TakeWebcamPictureHandler(const TakeWebcamPictureHandler&) = delete;
	TakeWebcamPictureHandler& operator=(const TakeWebcamPictureHandler&) = delete;
	TakeWebcamPictureHandler(TakeWebcamPictureHandler&&) = delete;
	TakeWebcamPictureHandler& operator=(TakeWebcamPictureHandler&&) = delete;

	void run() override;

private:
	uint32_t m_device_index;
};
