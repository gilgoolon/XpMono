#pragma once
#include "IFileIterator.hpp"
#include "RecursiveFileIterator.hpp"
#include "VolumeIterator.hpp"

#include <memory>

class FullFilesystemIterator final : public IFileIterator
{
public:
	explicit FullFilesystemIterator();
	explicit FullFilesystemIterator(uint32_t depth);
	~FullFilesystemIterator() override = default;
	FullFilesystemIterator(const FullFilesystemIterator&) = delete;
	FullFilesystemIterator& operator=(const FullFilesystemIterator&) = delete;
	FullFilesystemIterator(FullFilesystemIterator&&) = delete;
	FullFilesystemIterator& operator=(FullFilesystemIterator&&) = delete;

	[[nodiscard]] bool has_next() const override;
	[[nodiscard]] std::unique_ptr<FileEntry> next() override;

private:
	std::unique_ptr<VolumeIterator> m_volume_iterator;
	std::unique_ptr<RecursiveFileIterator> m_file_iterator;
	uint32_t m_max_depth;

	void try_retrieve_next();
};
