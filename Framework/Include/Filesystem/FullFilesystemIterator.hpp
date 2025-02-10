#pragma once
#include "RecursiveFileIterator.hpp"
#include "VolumeIterator.hpp"

#include <memory>

class FullFilesystemIterator final
{
public:
	explicit FullFilesystemIterator();
	~FullFilesystemIterator() = default;
	FullFilesystemIterator(const FullFilesystemIterator&) = delete;
	FullFilesystemIterator& operator=(const FullFilesystemIterator&) = delete;
	FullFilesystemIterator(FullFilesystemIterator&&) = delete;
	FullFilesystemIterator& operator=(FullFilesystemIterator&&) = delete;

	[[nodiscard]] bool has_next() const;
	[[nodiscard]] std::unique_ptr<FileEntry> next();

private:
	std::unique_ptr<VolumeIterator> m_volume_iterator;
	std::unique_ptr<RecursiveFileIterator> m_file_iterator;

	void try_retrieve_next();
};
