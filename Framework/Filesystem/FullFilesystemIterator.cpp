#include "Filesystem/FullFilesystemIterator.hpp"

#include "Exception.hpp"

FullFilesystemIterator::FullFilesystemIterator():
	FullFilesystemIterator((std::numeric_limits<uint32_t>::max)())
{
}

FullFilesystemIterator::FullFilesystemIterator(const uint32_t depth):
	m_volume_iterator(std::make_unique<VolumeIterator>()),
	m_file_iterator(std::make_unique<RecursiveFileIterator>(m_volume_iterator->next(), depth)),
	m_max_depth(depth)
{
	try_retrieve_next();
}

bool FullFilesystemIterator::has_next() const
{
	return m_file_iterator != nullptr && m_file_iterator->has_next();
}

std::unique_ptr<FileEntry> FullFilesystemIterator::next()
{
	if (!has_next())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}
	std::unique_ptr<FileEntry> result = m_file_iterator->next();
	try_retrieve_next();
	return std::move(result);
}

void FullFilesystemIterator::try_retrieve_next()
{
	while (!m_file_iterator->has_next() && m_volume_iterator->has_next())
	{
		m_file_iterator = std::make_unique<RecursiveFileIterator>(m_volume_iterator->next(), m_max_depth);
	}
}
