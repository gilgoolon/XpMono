#pragma once
#include "RelocationBlocksIterator.hpp"

namespace Pe
{
class RelocationEntriesIterator final
{
public:
	explicit RelocationEntriesIterator(const RelocationBlocksIterator::Entry& block);
	~RelocationEntriesIterator() = default;
	RelocationEntriesIterator(const RelocationEntriesIterator&) = delete;
	RelocationEntriesIterator& operator=(const RelocationEntriesIterator&) = delete;
	RelocationEntriesIterator(RelocationEntriesIterator&&) = delete;
	RelocationEntriesIterator& operator=(RelocationEntriesIterator&&) = delete;

	enum class Type : uint16_t
	{
		REL_ABSOLUTE = IMAGE_REL_BASED_ABSOLUTE,
		REL_HIGHLOW = IMAGE_REL_BASED_HIGHLOW,
	};

	struct Entry final
	{
		Type type;
		const uint32_t* address;
	};

	[[nodiscard]] bool has_next() const;

	[[nodiscard]] Entry next();

private:
	const uint16_t* m_current_entry;
	uint32_t m_calls_left;
	const void* m_block_rva;
};
}
