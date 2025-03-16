#include "GetFileHandler.hpp"

#include "Filesystem/File.hpp"
#include "Filesystem/FileContentIterator.hpp"
#include "Products/RawTypedProduct.hpp"
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

GetFileHandler::GetFileHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters) :
	GetFileHandler(std::move(operation_event), Json::parse(Strings::to_string(parameters)))
{
}

void GetFileHandler::run()
{
	FileContentIterator iterator(File(m_path, File::Mode::READ, File::Disposition::OPEN));
	Buffer result;
	while (iterator.has_next())
	{
		const Buffer contents = iterator.next();
		result.insert(result.end(), contents.begin(), contents.end());
	}
	append(std::make_unique<RawTypedProduct>(std::move(result)));
	finished();
}
