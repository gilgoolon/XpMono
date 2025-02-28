#include "GetFileHandler.hpp"

#include "Filesystem/File.hpp"
#include "Filesystem/FileContentIterator.hpp"
#include "Utils/Strings.hpp"

namespace Parameters
{
static constexpr auto PATH = "path";
}

GetFileHandler::GetFileHandler(std::unique_ptr<Event> operation_event, const Json& parameters) :
	IOperationHandler(std::move(operation_event)),
	m_path(parameters[Parameters::PATH].get<std::filesystem::path>())
{
}

GetFileHandler::GetFileHandler(std::unique_ptr<Event> operation_event, const Buffer &parameters) :
	GetFileHandler(std::move(operation_event), Json::parse(Strings::to_string(parameters)))
{
}

void GetFileHandler::run()
{
	FileContentIterator iterator(File(m_path, File::Mode::READ, File::Disposition::OPEN));
	while (iterator.has_next())
	{
		append(iterator.next());
	}
	finished();
}
