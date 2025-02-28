#include "DirlistHandler.hpp"

#include "Filesystem/FileIterator.hpp"
#include "Filesystem/FullFilesystemIterator.hpp"
#include "Filesystem/RecursiveFileIterator.hpp"
#include "Utils/Strings.hpp"

#include <filesystem>

namespace Parameters
{
static constexpr auto PATH = "path";
static constexpr auto DEPTH = "depth";
}

DirlistHandler::DirlistHandler(std::unique_ptr<Event> operation_event, const Json& parameters):
	IOperationHandler(std::move(operation_event)),
	m_path(get_optional<std::filesystem::path>(parameters, Parameters::PATH)),
	m_depth(get_optional<uint32_t>(parameters, Parameters::DEPTH))
{
}

DirlistHandler::DirlistHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters) :
	DirlistHandler(std::move(operation_event), Json::parse(Strings::to_string(parameters)))
{
}

void DirlistHandler::run()
{
	static constexpr auto SUFFIX = L"\n";
	const Buffer suffix = Strings::to_buffer(SUFFIX);
	// todo: make IFileIterator interface and use arguments
	std::unique_ptr<IFileIterator> iterator = make_iterator(m_path, m_depth);
	while (iterator->has_next())
	{
		const std::unique_ptr<FileEntry> file = iterator->next();
		append(Strings::concat(file->serialize(), suffix));
	}
	finished();
}

std::unique_ptr<IFileIterator> DirlistHandler::make_iterator(const std::optional<std::filesystem::path>& path,
                                                             std::optional<uint32_t> depth)
{
	if (!path.has_value())
	{
		return depth.has_value()
			       ? std::make_unique<FullFilesystemIterator>(depth.value())
			       : std::make_unique<FullFilesystemIterator>();
	}
	return depth.has_value()
		       ? std::make_unique<RecursiveFileIterator>(path.value(), depth.value())
		       : std::make_unique<RecursiveFileIterator>(path.value());
}
