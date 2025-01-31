#pragma once
#include <cstdint>

namespace Pe
{
class ExportedFunctionsIterator final
{
public:
	explicit ExportedFunctionsIterator(const void* module, bool& result);
	~ExportedFunctionsIterator() = default;
	ExportedFunctionsIterator(const ExportedFunctionsIterator&) = delete;
	ExportedFunctionsIterator& operator=(const ExportedFunctionsIterator&) = delete;
	ExportedFunctionsIterator(ExportedFunctionsIterator&&) = delete;
	ExportedFunctionsIterator& operator=(ExportedFunctionsIterator&&) = delete;

private:
	const char* get_function_name(uint16_t ordinal) const;

public:
	struct Entry final
	{
		uint32_t rva;
		uint16_t ordinal;
		const char* name;
	};

	[[nodiscard]] bool has_next() const;

	[[nodiscard]] bool next(Entry& result);

private:
	const void* m_module;
	uint32_t m_num_functions;
	const uint32_t* m_functions;
	uint32_t m_num_names;
	const uint32_t* m_names;
	const uint16_t* m_name_ordinals;
	uint32_t m_base;
	mutable uint32_t m_current_index;
};
}
