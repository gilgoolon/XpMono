#include "GetFileHandler.hpp"

#include "Filesystem/File.hpp"
#include "Filesystem/FileContentIterator.hpp"

namespace Parameters
{
static constexpr std::string_view PATH = "path";
}

GetFileHandler::GetFileHandler(std::unique_ptr<Event> operation_event, const Json& parameters) :
	IOperationHandler(std::move(operation_event)),
	m_path(parameters[std::string{Parameters::PATH}].get<std::filesystem::path>())
{
}

GetFileHandler::GetFileHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters):
	GetFileHandler(std::move(operation_event), Json::from_bson(parameters))
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
