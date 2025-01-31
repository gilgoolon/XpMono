#pragma once
#include "HeapMemory.hpp"

#include <cstdint>

class RelocationBlocksIterator final
{
public:
	explicit RelocationBlocksIterator(const void* module, bool& result);
	~RelocationBlocksIterator() = default;
	RelocationBlocksIterator(const RelocationBlocksIterator&) = delete;
	RelocationBlocksIterator& operator=(const RelocationBlocksIterator&) = delete;
	RelocationBlocksIterator(RelocationBlocksIterator&&) = delete;
	RelocationBlocksIterator& operator=(RelocationBlocksIterator&&) = delete;

	struct Entry final
	{
		const void* virtual_address;
		uint32_t items;
		const uint16_t* start_of_block;
	};

	[[nodiscard]] bool has_next() const;

	[[nodiscard]] Entry next();

private:
	const void* m_module;
	const IMAGE_BASE_RELOCATION* m_current_block;
	uint32_t m_current_offset;
	uint32_t m_size;
};
