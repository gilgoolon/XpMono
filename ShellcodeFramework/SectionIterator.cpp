#include "SectionIterator.hpp"

Pe::SectionIterator::SectionIterator(const void* module, bool& result):
	m_module(module),
	m_entry(),
	m_next_section(get_section_table(module)),
	m_calls_left(0)
{
	const IMAGE_FILE_HEADER* const file_header = get_file_header(module);
	const IMAGE_OPTIONAL_HEADER* const optional_header = get_optional_header(module);
	if (file_header == nullptr || optional_header == nullptr || m_next_section == nullptr)
	{
		result = false;
		return;
	}
	m_entry = {module, optional_header->SizeOfHeaders, 0, optional_header->SizeOfHeaders, PAGE_READONLY};
	m_calls_left = file_header->NumberOfSections + 1;
}

bool Pe::SectionIterator::has_next() const
{
	return m_calls_left > 0;
}

static DWORD characteristics_to_permissions(const DWORD characteristics)
{
	DWORD result = 0;

	const bool is_executable = (characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
	const bool is_readable = (characteristics & IMAGE_SCN_MEM_READ) != 0;
	const bool is_writable = (characteristics & IMAGE_SCN_MEM_WRITE) != 0;

	if (is_executable)
	{
		if (is_readable && is_writable)
		{
			result = PAGE_EXECUTE_READWRITE;
		}
		else if (is_readable)
		{
			result = PAGE_EXECUTE_READ;
		}
		else if (is_writable)
		{
			result = PAGE_EXECUTE_WRITECOPY;
		}
		else
		{
			result = PAGE_EXECUTE;
		}
	}
	else
	{
		if (is_readable && is_writable)
		{
			result = PAGE_READWRITE;
		}
		else if (is_readable)
		{
			result = PAGE_READONLY;
		}
		else if (is_writable)
		{
			result = PAGE_WRITECOPY;
		}
		else
		{
			result = PAGE_NOACCESS;
		}
	}

	return result;
}

Pe::SectionIterator::Entry Pe::SectionIterator::next()
{
	--m_calls_left;
	const Entry result = m_entry;
	if (m_calls_left > 0)
	{
		const IMAGE_SECTION_HEADER* const next_section = m_next_section++;
		m_entry = {
			static_cast<const uint8_t*>(m_module) + next_section->PointerToRawData,
			next_section->SizeOfRawData,
			next_section->VirtualAddress,
			next_section->Misc.VirtualSize,
			characteristics_to_permissions(next_section->Characteristics)
		};
	}
	return result;
}
