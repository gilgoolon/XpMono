#include "TakeWebcamPictureHandler.hpp"

namespace Parameters
{
static constexpr auto DEVICE_INDEX = "device_index";
}

TakeWebcamPictureHandler::TakeWebcamPictureHandler(std::unique_ptr<Event> operation_event,
                                                   const uint32_t device_index) :
	IOperationHandler(std::move(operation_event)),
	m_device_index(device_index)
{
}

TakeWebcamPictureHandler::TakeWebcamPictureHandler(std::unique_ptr<Event> operation_event, const Json& parameters) :
	TakeWebcamPictureHandler(std::move(operation_event), parameters[Parameters::DEVICE_INDEX].get<uint32_t>())
{
}

TakeWebcamPictureHandler::TakeWebcamPictureHandler(std::unique_ptr<Event> operation_event,
                                                   const Buffer& raw_parameters) :
	TakeWebcamPictureHandler(std::move(operation_event), Json::parse(Strings::to_string(raw_parameters)))
{
}

void TakeWebcamPictureHandler::run()
{
}
