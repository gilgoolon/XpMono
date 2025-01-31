#include "Pe/RelocationBlocksIterator.hpp"

#include "Pe/Pe.hpp"

Pe::RelocationBlocksIterator::RelocationBlocksIterator(const void* module, bool& result):
	m_module(module),
	m_current_block(nullptr),
	m_current_offset(0),
	m_size(0)
{
	result = false;
	const IMAGE_DATA_DIRECTORY* const relocations_data_directory = get_data_directory(
		module,
		IMAGE_DIRECTORY_ENTRY_BASERELOC
	);
	if (relocations_data_directory == nullptr)
	{
		result = false;
		return;
	}
	m_current_block = reinterpret_cast<const IMAGE_BASE_RELOCATION*>(
		static_cast<const uint8_t*>(module) + relocations_data_directory->VirtualAddress
	);
	m_size = relocations_data_directory->Size;
	result = true;
}

bool Pe::RelocationBlocksIterator::has_next() const
{
	return m_current_offset < m_size;
}

Pe::RelocationBlocksIterator::Entry Pe::RelocationBlocksIterator::next()
{
	const Entry result = {
		static_cast<const uint8_t*>(m_module) + m_current_block->VirtualAddress,
		(m_current_block->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(uint16_t),
		reinterpret_cast<const uint16_t*>(reinterpret_cast<const uint8_t*>(m_current_block) + sizeof(
			IMAGE_BASE_RELOCATION))
	};
	m_current_offset += m_current_block->SizeOfBlock;
	m_current_block = reinterpret_cast<const IMAGE_BASE_RELOCATION*>(reinterpret_cast<const uint8_t*>(m_current_block) +
		m_current_block->SizeOfBlock);
	return result;
}
