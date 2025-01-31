#pragma once
#include "Pe.hpp"

#include <cstdint>

namespace Pe
{
class SectionIterator final
{
public:
	struct Entry final
	{
		const void* raw_data;
		uint32_t raw_size;
		uint32_t rva;
		uint32_t virtual_size;
		DWORD permissions;
	};

	explicit SectionIterator(const void* module, bool& result);
	~SectionIterator() = default;
	SectionIterator(const SectionIterator&) = delete;
	SectionIterator& operator=(const SectionIterator&) = delete;
	SectionIterator(SectionIterator&&) = delete;
	SectionIterator& operator=(SectionIterator&&) = delete;

	bool has_next() const;

	Entry next();

private:
	const void* m_module;
	Entry m_entry;
	const IMAGE_SECTION_HEADER* m_next_section;
	uint32_t m_calls_left;
};
}
