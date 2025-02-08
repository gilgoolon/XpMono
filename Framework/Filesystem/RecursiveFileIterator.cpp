#include "Filesystem/RecursiveFileIterator.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Filesystem/FileIterator.hpp"

RecursiveFileIterator::RecursiveFileIterator(const std::filesystem::path& root):
	m_entries()
{
	append_next(root);
}

bool RecursiveFileIterator::has_next() const
{
	return !m_entries.empty();
}

std::unique_ptr<FileEntry> RecursiveFileIterator::next()
{
	if (m_entries.empty())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}
	std::unique_ptr<FileEntry> result = std::move(m_entries.front());
	m_entries.pop();
	if (result->m_type == FileEntry::Type::DIRECTORY)
	{
		append_next(result->m_path);
	}
	return std::move(result);
}

void RecursiveFileIterator::append_next(const std::filesystem::path& path)
{
	try
	{
		for (FileIterator iterator(path); iterator.has_next();)
		{
			m_entries.push(iterator.next());
		}
	}
	catch (const WinApiException& exception)
	{
		TRACE(exception.error())
		if (exception.error() == ERROR_ACCESS_DENIED)
		{
			return;
		}
		throw;
	}
}
