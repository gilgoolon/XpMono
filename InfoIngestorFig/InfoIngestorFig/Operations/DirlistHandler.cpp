#include "DirlistHandler.hpp"

#include "Filesystem/FileIterator.hpp"
#include "Filesystem/FullFilesystemIterator.hpp"
#include "Filesystem/RecursiveFileIterator.hpp"
#include "Utils/Strings.hpp"

#include <filesystem>

namespace Parameters
{
static constexpr std::wstring_view PATH = L"path";
static constexpr std::wstring_view DEPTH = L"depth";
}

DirlistHandler::DirlistHandler(std::unique_ptr<Event> operation_event, const json& parameters):
	IOperationHandler(std::move(operation_event)),
	m_path(parameters[std::wstring{Parameters::PATH}]),
	m_depth(parameters[std::wstring{Parameters::DEPTH}])
{
}

DirlistHandler::DirlistHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters) :
	DirlistHandler(std::move(operation_event), Strings::to_wstring(parameters))
{
}

void DirlistHandler::run()
{
	static constexpr std::wstring_view SUFFIX = L"\n";
	const Buffer suffix = Strings::to_buffer(std::wstring{SUFFIX});
	for (FullFilesystemIterator iterator; iterator.has_next();)
	{
		const std::unique_ptr<FileEntry> file = iterator.next();
		append(Strings::concat(file->serialize(), suffix));
	}
	finished();
}
