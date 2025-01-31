#include "Pe/RelocationEntriesIterator.hpp"

Pe::RelocationEntriesIterator::RelocationEntriesIterator(const RelocationBlocksIterator::Entry& block):
	m_current_entry(block.start_of_block),
	m_calls_left(block.items),
	m_block_rva(block.virtual_address)
{
}

bool Pe::RelocationEntriesIterator::has_next() const
{
	return m_calls_left > 0;
}

Pe::RelocationEntriesIterator::Entry Pe::RelocationEntriesIterator::next()
{
	--m_calls_left;
	static constexpr uint32_t OFFSET_MASK = 0x0fff;
	static constexpr uint32_t TYPE_SHIFT = 12;
	const Entry result = {
		static_cast<Type>(*m_current_entry >> TYPE_SHIFT),
		reinterpret_cast<const uint32_t*>(static_cast<const uint8_t*>(m_block_rva) + (*m_current_entry & OFFSET_MASK))
	};
	++m_current_entry;
	return result;
}
