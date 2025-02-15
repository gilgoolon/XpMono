#pragma once
#include "Filesystem/File.hpp"

class FileContentIterator final
{
public:
	static constexpr uint32_t DEFAULT_BUFF_SIZE = 4096;
	explicit FileContentIterator(File file, uint32_t buff_size = DEFAULT_BUFF_SIZE);
	~FileContentIterator() = default;
	FileContentIterator(const FileContentIterator&) = delete;
	FileContentIterator& operator=(const FileContentIterator&) = delete;
	FileContentIterator(FileContentIterator&&) = delete;
	FileContentIterator& operator=(FileContentIterator&&) = delete;

	[[nodiscard]] bool has_next() const;

	[[nodiscard]] Buffer next();

private:
	const uint32_t m_buff_size;
	File m_file;
	mutable std::optional<Buffer> m_next_data;
};
