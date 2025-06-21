#include "TakePictureHandler.hpp"

#include "Media/MediaFoundation/Attributes.hpp"
#include "Media/MediaFoundation/Instance.hpp"
#include "Products/ImageBmpTypedProduct.hpp"

namespace Parameters
{
static constexpr auto DEVICE_INDEX = "device_index";
static constexpr uint32_t DEVICE_INDEX_DEFAULT = 0;
}

TakePictureHandler::TakePictureHandler(std::unique_ptr<Event> operation_event,
                                       const uint32_t device_index) :
	IOperationHandler(std::move(operation_event)),
	m_device_index(device_index)
{
}

TakePictureHandler::TakePictureHandler(std::unique_ptr<Event> operation_event, const Json& parameters) :
	TakePictureHandler(
		std::move(operation_event),
		parameters.value(Parameters::DEVICE_INDEX, Parameters::DEVICE_INDEX_DEFAULT)
	)
{
}

TakePictureHandler::TakePictureHandler(std::unique_ptr<Event> operation_event,
                                       const Buffer& raw_parameters) :
	TakePictureHandler(std::move(operation_event), Json::parse(Strings::to_string(raw_parameters)))
{
}

void TakePictureHandler::run()
{
	MediaFoundation::Instance instance;

	MediaFoundation::Attributes attributes;
	attributes.set_source_type(MediaFoundation::MediaType::Type::VIDEO);

	std::vector<std::unique_ptr<MediaFoundation::Device>> devices = attributes.enumerate_devices();

	if (m_device_index >= devices.size())
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}

	std::unique_ptr<MediaFoundation::Device>& device = devices[m_device_index];

	MediaFoundation::MediaSource source = device->activate();

	MediaFoundation::SourceReader reader = source.create_reader();
	reader.set_media_type(
		MediaFoundation::MediaType(
			MediaFoundation::MediaType::Type::VIDEO,
			MediaFoundation::MediaType::SubType::VIDEO_FORMAT_RGB32
		)
	);

	Buffer result_buffer;

	static constexpr size_t RETRIES_COUNT = 10;
	static constexpr Time::Millis RETRY_DELAY{50};

	for (size_t i = 0; i < RETRIES_COUNT; ++i)
	{
		std::optional<MediaFoundation::Sample> sample = reader.read_sample();
		if (!sample.has_value())
		{
			IWaitable::sleep(RETRY_DELAY);
			continue;
		}

		const MediaFoundation::MediaBuffer media_buffer = sample->to_contiguous_buffer();
		result_buffer = media_buffer.to_buffer();
		break;
	}

	append(std::make_unique<ImageBmpTypedProduct>(std::move(result_buffer)));
	finished();
}
