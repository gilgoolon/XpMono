#include "DirlistHandler.hpp"

#include "Filesystem/FileIterator.hpp"
#include "Filesystem/FullFilesystemIterator.hpp"
#include "Filesystem/RecursiveFileIterator.hpp"
#include "Utils/Strings.hpp"

#include <filesystem>

namespace Parameters
{
static constexpr std::string_view PATH = "path";
static constexpr std::string_view DEPTH = "depth";
}

DirlistHandler::DirlistHandler(std::unique_ptr<Event> operation_event, const Json& parameters):
	IOperationHandler(std::move(operation_event)),
	m_path(get_optional<std::filesystem::path>(parameters, std::string{Parameters::PATH})),
	m_depth(get_optional<uint32_t>(parameters, std::string{Parameters::DEPTH}))
{
}

DirlistHandler::DirlistHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters) :
	DirlistHandler(std::move(operation_event), Json::from_bson(parameters))
{
}

void DirlistHandler::run()
{
	static constexpr std::wstring_view SUFFIX = L"\n";
	const Buffer suffix = Strings::to_buffer(std::wstring{SUFFIX});
	// todo: make IFileIterator interface and use arguments
	for (FullFilesystemIterator iterator; iterator.has_next();)
	{
		const std::unique_ptr<FileEntry> file = iterator.next();
		append(Strings::concat(file->serialize(), suffix));
	}
	finished();
}
