#pragma once
#include "IFileIterator.hpp"

#include <memory>

class FileIterator final : public IFileIterator
{
public:
	explicit FileIterator(const std::filesystem::path& folder);
	~FileIterator() override;
	FileIterator(const FileIterator&) = delete;
	FileIterator& operator=(const FileIterator&) = delete;
	FileIterator(FileIterator&&) = delete;
	FileIterator& operator=(FileIterator&&) = delete;

	[[nodiscard]] bool has_next() const override;
	[[nodiscard]] std::unique_ptr<FileEntry> next() override;

private:
	std::filesystem::path m_folder;
	HANDLE m_handle;
	std::unique_ptr<FileEntry> m_next_entry;

	void retrieve_next();
	[[nodiscard]] static bool is_ignored(const FileEntry& file);
};
