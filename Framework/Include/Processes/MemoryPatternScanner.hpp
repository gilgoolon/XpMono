#pragma once
#include "Process.hpp"
#include "ProcessMemoryIterator.hpp"
#include <functional>
#include <vector>

class MemoryPatternScanner
{
public:
	using MemoryFilter = std::function<bool(const Process& process, const void* address)>;

	explicit MemoryPatternScanner(const Process& process, const std::vector<MemoryFilter>& filters);
	~MemoryPatternScanner() = default;
	MemoryPatternScanner(const MemoryPatternScanner&) = delete;
	MemoryPatternScanner& operator=(const MemoryPatternScanner&) = delete;
	MemoryPatternScanner(MemoryPatternScanner&&) = delete;
	MemoryPatternScanner& operator=(MemoryPatternScanner&&) = delete;

	[[nodiscard]] std::vector<void*> get_all() const;
	[[nodiscard]] void* next() const;
	[[nodiscard]] bool has_next() const;

private:
	void retrieve_first() const;
	void retrieve_next() const;

	const Process& m_process;
	std::vector<MemoryFilter> m_filters;
	mutable void* m_next_result;
	mutable ProcessMemoryIterator m_memory_iterator;
};
