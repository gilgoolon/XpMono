#pragma once
#include "FileEntry.hpp"

#include <memory>

class IFileIterator
{
public:
	explicit IFileIterator() = default;
	virtual ~IFileIterator() = default;
	IFileIterator(const IFileIterator&) = delete;
	IFileIterator& operator=(const IFileIterator&) = delete;
	IFileIterator(IFileIterator&&) = delete;
	IFileIterator& operator=(IFileIterator&&) = delete;

	[[nodiscard]] virtual bool has_next() const = 0;
	[[nodiscard]] virtual std::unique_ptr<FileEntry> next() = 0;
};
