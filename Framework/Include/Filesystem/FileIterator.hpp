#pragma once
#include "FileEntry.hpp"

#include <memory>

class FileIterator final
{
public:
	explicit FileIterator(const std::filesystem::path& folder);
	~FileIterator();
	FileIterator(const FileIterator&) = delete;
	FileIterator& operator=(const FileIterator&) = delete;
	FileIterator(FileIterator&&) = delete;
	FileIterator& operator=(FileIterator&&) = delete;

	[[nodiscard]] bool has_next() const;
	[[nodiscard]] std::unique_ptr<FileEntry> next();

private:
	std::filesystem::path m_path;
	HANDLE m_handle;
	std::unique_ptr<FileEntry> m_next_entry;

	void retrieve_next();
	[[nodiscard]] static bool is_ignored(const FileEntry& file);
};
