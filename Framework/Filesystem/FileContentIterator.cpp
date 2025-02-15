#include "Filesystem/FileContentIterator.hpp"

#include "Exception.hpp"

FileContentIterator::FileContentIterator(File file, const uint32_t buff_size):
	m_buff_size(buff_size),
	m_file(std::move(file))
{
}

bool FileContentIterator::has_next() const
{
	if (m_next_data.has_value() && m_next_data->empty())
	{
		return false;
	}
	if (!m_next_data.has_value())
	{
		m_next_data = m_file.read(m_buff_size);
	}
	return !m_next_data->empty();
}

Buffer FileContentIterator::next()
{
	if (!has_next())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}
	Buffer result = std::move(m_next_data.value());
	m_next_data.reset();
	return std::move(result);
}
