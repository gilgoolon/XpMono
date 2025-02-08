#include "Filesystem/FileIterator.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

FileIterator::FileIterator(const std::filesystem::path& folder):
	m_handle(nullptr),
	m_next_entry(nullptr)
{
	static constexpr std::wstring_view ANY_FILE_SUFFIX = L"*";
	const std::filesystem::path expression = folder / std::wstring{ANY_FILE_SUFFIX};
	WIN32_FIND_DATAW data{};
	m_handle = FindFirstFileW(expression.wstring().c_str(), &data);
	if (m_handle == INVALID_HANDLE_VALUE)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_ITERATOR_START);
	}
	m_next_entry = std::make_unique<FileEntry>(
		folder,
		Buffer{reinterpret_cast<const uint8_t*>(&data), reinterpret_cast<const uint8_t*>(&data) + sizeof data}
	);
	retrieve_next();
}

FileIterator::~FileIterator()
{
	try
	{
		if (FindClose(m_handle) == FALSE)
		{
			TRACE(L"failed to close file iterator")
		}
	}
	catch (...)
	{
		TRACE(L"failed to close file iterator")
	}
}

bool FileIterator::has_next() const
{
	return m_next_entry != nullptr;
}

std::unique_ptr<FileEntry> FileIterator::next()
{
	if (m_next_entry == nullptr)
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}
	std::unique_ptr<FileEntry> result = std::move(m_next_entry);
	m_next_entry = nullptr;
	retrieve_next();
	return std::move(result);
}

void FileIterator::retrieve_next()
{
	while (m_next_entry == nullptr || is_ignored(*m_next_entry))
	{
		WIN32_FIND_DATAW data{};
		if (FindNextFileW(m_handle, &data) == FALSE)
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
			{
				break;
			}
			throw WinApiException(ErrorCode::FAILED_FILE_ITERATOR_NEXT);
		}
		m_next_entry = std::make_unique<FileEntry>(
			m_path,
			Buffer{reinterpret_cast<const uint8_t*>(&data), reinterpret_cast<const uint8_t*>(&data) + sizeof data}
		);
	}
}

bool FileIterator::is_ignored(const FileEntry& file)
{
	const std::wstring filename = file.m_path.filename();
	return filename == L".." || filename == L".";
}
