#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"
#include "Synchronization/Event.hpp"

class TakePictureHandler final : public IOperationHandler
{
public:
	explicit TakePictureHandler(std::unique_ptr<Event> operation_event, uint32_t device_index);
	explicit TakePictureHandler(std::unique_ptr<Event> operation_event, const Json& parameters);
	explicit TakePictureHandler(std::unique_ptr<Event> operation_event, const Buffer& raw_parameters);
	~TakePictureHandler() override = default;
	TakePictureHandler(const TakePictureHandler&) = delete;
	TakePictureHandler& operator=(const TakePictureHandler&) = delete;
	TakePictureHandler(TakePictureHandler&&) = delete;
	TakePictureHandler& operator=(TakePictureHandler&&) = delete;

	void run() override;

private:
	uint32_t m_device_index;
};
