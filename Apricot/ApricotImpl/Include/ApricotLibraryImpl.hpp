#pragma once
#include "ApricotCode.hpp"
#include "HeapMemory.hpp"
#include "Pe/SectionIterator.hpp"

#include <cstdint>

class ApricotLibraryImpl final
{
public:
	explicit ApricotLibraryImpl(const uint8_t* unloaded_module, uint32_t size, ApricotCode& result);
	~ApricotLibraryImpl();
	ApricotLibraryImpl(const ApricotLibraryImpl&) = delete;
	ApricotLibraryImpl& operator=(const ApricotLibraryImpl&) = delete;
	ApricotLibraryImpl(ApricotLibraryImpl&&) = delete;
	ApricotLibraryImpl& operator=(ApricotLibraryImpl&&) = delete;

private:
	void map_section_entry(const Pe::SectionIterator::Entry& entry);
	bool map_sections(const void* module);
	[[nodiscard]] bool finalize_section_entry(const Pe::SectionIterator::Entry& entry, uint32_t section_alignment);
	bool finalize_sections();

	using DllEntryPoint = BOOL(WINAPI*)(HMODULE, DWORD, DWORD);
	bool call_entry_point(DWORD reason, BOOL& return_value);

	bool load_module(const IMAGE_IMPORT_DESCRIPTOR* module);
	bool load_dependencies();

	void unload_dependency(const IMAGE_IMPORT_DESCRIPTOR* module);
	void unload_dependencies(uint32_t loaded_modules);
	void unload_dependencies();

public:
	[[nodiscard]] ApricotCode get_proc_address(uint16_t ordinal, void*& result) const;
	[[nodiscard]] ApricotCode get_proc_address(const char* name, void*& result) const;

private:
	bool m_is_initialized;
	Shellcode::HeapMemory m_memory;
};
