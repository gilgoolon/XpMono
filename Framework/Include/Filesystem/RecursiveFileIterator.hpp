#pragma once
#include "FileEntry.hpp"

#include <filesystem>
#include <queue>

class RecursiveFileIterator final
{
public:
	explicit RecursiveFileIterator(const std::filesystem::path& root);
	~RecursiveFileIterator() = default;
	RecursiveFileIterator(const RecursiveFileIterator&) = delete;
	RecursiveFileIterator& operator=(const RecursiveFileIterator&) = delete;
	RecursiveFileIterator(RecursiveFileIterator&&) = delete;
	RecursiveFileIterator& operator=(RecursiveFileIterator&&) = delete;

	[[nodiscard]] bool has_next() const;
	[[nodiscard]] std::unique_ptr<FileEntry> next();

private:
	std::queue<std::unique_ptr<FileEntry>> m_entries;

	void append_next(const std::filesystem::path& path);
};
