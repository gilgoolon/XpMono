#pragma once
#include "FileEntry.hpp"
#include "IFileIterator.hpp"

#include <filesystem>
#include <queue>

class RecursiveFileIterator final : public IFileIterator
{
public:
	explicit RecursiveFileIterator(const std::filesystem::path& root);
	explicit RecursiveFileIterator(const std::filesystem::path& root, uint32_t depth);
	~RecursiveFileIterator() override = default;
	RecursiveFileIterator(const RecursiveFileIterator&) = delete;
	RecursiveFileIterator& operator=(const RecursiveFileIterator&) = delete;
	RecursiveFileIterator(RecursiveFileIterator&&) = delete;
	RecursiveFileIterator& operator=(RecursiveFileIterator&&) = delete;

	[[nodiscard]] bool has_next() const override;
	[[nodiscard]] std::unique_ptr<FileEntry> next() override;

private:
	std::queue<std::unique_ptr<FileEntry>> m_entries;
	uint32_t m_depth_left;

	void append_next(const std::filesystem::path& path);
};
