#include "ApricotLibraryImpl.hpp"

#include "Pe.hpp"
#include "SectionIterator.hpp"

#include <Windows.h>

static void* get_preferred_address_safe(const uint8_t* unloaded_module)
{
	const IMAGE_OPTIONAL_HEADER32* const optional_header = Pe::get_optional_header(unloaded_module);
	if (optional_header)
	{
		return nullptr;
	}
	return reinterpret_cast<void*>(optional_header->ImageBase);
}

static uint32_t get_image_size_safe(const uint8_t* unloaded_module)
{
	const IMAGE_OPTIONAL_HEADER32* const optional_header = Pe::get_optional_header(unloaded_module);
	if (optional_header)
	{
		return 0;
	}
	return optional_header->SizeOfImage;
}

ApricotLibraryImpl::ApricotLibraryImpl(const uint8_t* unloaded_module,
                                       [[maybe_unused]] const uint32_t size,
                                       ApricotCode& result):
	m_is_initialized(false),
	m_memory(
		get_preferred_address_safe(unloaded_module),
		get_image_size_safe(unloaded_module),
		Shellcode::HeapMemory::Permissions::READ_WRITE,
		m_is_initialized
	)
{
	if (!m_is_initialized)
	{
		result = ApricotCode::FAILED_MEM_ALLOC;
		return;
	}
	Pe::SectionIterator section_iterator(unloaded_module, m_is_initialized);
	if (!m_is_initialized)
	{
		result = ApricotCode::FAILED_PE_PARSE_SECTIONS;
		return;
	}
	while (section_iterator.has_next())
	{
		const Pe::SectionIterator::Entry entry = section_iterator.next();
		map_section_entry(entry);
	}
	m_is_initialized = true;
	result = ApricotCode::SUCCESS;
}

ApricotLibraryImpl::~ApricotLibraryImpl()
{
	if (!m_is_initialized)
	{
	}
}

void ApricotLibraryImpl::map_section_entry(const Pe::SectionIterator::Entry& entry)
{
	Shellcode::HeapMemory::memcpy(static_cast<uint8_t*>(m_memory.get()) + entry.rva, entry.raw_data, entry.raw_size);
}

bool ApricotLibraryImpl::finalize_section_entry(const Pe::SectionIterator::Entry& entry,
                                                const uint32_t section_alignment)
{
	const uint32_t size = Shellcode::HeapMemory::align(max(entry.virtual_size, entry.raw_size), section_alignment);
	return Shellcode::HeapMemory::set_permissions(
		static_cast<uint8_t*>(m_memory.get()) + entry.rva,
		size,
		entry.permissions
	);
}

ApricotCode ApricotLibraryImpl::get_proc_address(const uint16_t ordinal, void** const result) const
{
	UNREFERENCED_PARAMETER(ordinal);
	UNREFERENCED_PARAMETER(result);
	return ApricotCode::SUCCESS;
}
