#include "Filesystem/RecursiveFileIterator.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Filesystem/FileIterator.hpp"

RecursiveFileIterator::RecursiveFileIterator(const std::filesystem::path& root):
	RecursiveFileIterator(root, (std::numeric_limits<uint32_t>::max)())
{
}

RecursiveFileIterator::RecursiveFileIterator(const std::filesystem::path& root, const uint32_t depth):
	m_entries(),
	m_depth_left(depth)
{
	m_entries.push(nullptr);
	append_next(root);
}

bool RecursiveFileIterator::has_next() const
{
	static constexpr size_t ONLY_HAS_NULL_DESCRIPTOR = 1;
	return !m_entries.empty() && m_entries.size() > ONLY_HAS_NULL_DESCRIPTOR;
}

std::unique_ptr<FileEntry> RecursiveFileIterator::next()
{
	if (m_entries.empty())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}
	std::unique_ptr<FileEntry> result = std::move(m_entries.front());
	m_entries.pop();
	if (result == nullptr)
	{
		--m_depth_left;
		m_entries.push(nullptr);
		result = std::move(m_entries.front());
		m_entries.pop();
	}
	if (result == nullptr)
	{
		throw Exception(ErrorCode::ILLEGAL_OBJECT_STATE);
	}
	if (result->m_type == FileEntry::Type::DIRECTORY)
	{
		append_next(result->m_path);
	}
	return std::move(result);
}

void RecursiveFileIterator::append_next(const std::filesystem::path& path)
{
	if (m_depth_left <= 0)
	{
		return;
	}
	try
	{
		for (FileIterator iterator(path); iterator.has_next();)
		{
			m_entries.push(iterator.next());
		}
	}
	catch (const WinApiException& exception)
	{
		TRACE(L"failed to open directory ", path, " with error ", GetLastError());
		if (exception.error() == ERROR_ACCESS_DENIED)
		{
			return;
		}
		throw;
	}
}
