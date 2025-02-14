#include "DropFileHandler.hpp"

#include "Filesystem/File.hpp"

namespace Parameters
{
static constexpr std::string_view DESTINATION = "destination";
static constexpr std::string_view DATA = "data";
static constexpr std::string_view METADATA_FILE = "metadata_file";
}

DropFileHandler::DropFileHandler(std::unique_ptr<Event> operation_event,
                                 const Json& parameters):
	IOperationHandler(std::move(operation_event)),
	m_destination(parameters[std::string{Parameters::DESTINATION}]),
	m_data(parameters[std::string{Parameters::DATA}]),
	m_metadata_file(get_optional<std::filesystem::path>(parameters, std::string{Parameters::METADATA_FILE}))
{
}

DropFileHandler::DropFileHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters):
	DropFileHandler(std::move(operation_event), Json::from_bson(parameters))
{
}

void DropFileHandler::run()
{
	File destination(m_destination, File::Mode::WRITE, File::Disposition::OVERRIDE);
	destination.write(m_data);
	if (m_metadata_file.has_value())
	{
		destination.set_metadata_of(File(m_metadata_file.value(), File::Mode::READ, File::Disposition::OPEN));
	}
	finished();
}
