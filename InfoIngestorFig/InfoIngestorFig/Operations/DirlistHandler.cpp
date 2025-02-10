#include "DirlistHandler.hpp"

#include "Filesystem/FileIterator.hpp"
#include "Filesystem/FullFilesystemIterator.hpp"
#include "Filesystem/RecursiveFileIterator.hpp"
#include "Utils/Strings.hpp"

#include <filesystem>

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
