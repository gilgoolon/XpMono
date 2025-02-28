#include "DropFileHandler.hpp"

#include "Crypto/Base64.hpp"
#include "Filesystem/File.hpp"
#include "Utils/Strings.hpp"

namespace Parameters
{
static constexpr auto DESTINATION = "destination";
static constexpr auto DATA = "data";
static constexpr auto METADATA_FILE = "metadata_file";
}

DropFileHandler::DropFileHandler(std::unique_ptr<Event> operation_event,
                                 const Json& parameters):
	IOperationHandler(std::move(operation_event)),
	m_destination(parameters[Parameters::DESTINATION].get<std::filesystem::path>()),
	m_data(Base64::decode(parameters[Parameters::DATA].get<std::string>())),
	m_metadata_file(get_optional<std::filesystem::path>(parameters, Parameters::METADATA_FILE))
{
}

DropFileHandler::DropFileHandler(std::unique_ptr<Event> operation_event, const Buffer &parameters) :
	DropFileHandler(std::move(operation_event), Json::parse(Strings::to_string(parameters)))
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
