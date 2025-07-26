#pragma once
#include "Process.hpp"

class ProcessMemoryIterator
{
public:
	using RegionFilter = struct
	{
		uint32_t state;
		uint32_t type;
		uint32_t protect;
	};

	explicit ProcessMemoryIterator(const Process& process,
	                               const RegionFilter& filter = {MEM_COMMIT, MEM_PRIVATE, PAGE_READWRITE});
	~ProcessMemoryIterator() = default;
	ProcessMemoryIterator(const ProcessMemoryIterator&) = delete;
	ProcessMemoryIterator& operator=(const ProcessMemoryIterator&) = delete;
	ProcessMemoryIterator(ProcessMemoryIterator&&) = default;
	ProcessMemoryIterator& operator=(ProcessMemoryIterator&&) = default;

	[[nodiscard]] void* next() const;
	[[nodiscard]] bool has_next() const;

private:
	void retrieve_first() const;
	void retrieve_next() const;
	void next_region() const;

	const Process& m_process;
	const RegionFilter m_filter;
	mutable void* m_next_address;
	mutable MEMORY_BASIC_INFORMATION m_current_region_info;
};
