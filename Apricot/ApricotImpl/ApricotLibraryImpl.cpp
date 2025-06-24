#include "ApricotLibraryImpl.hpp"

#include "HeapMemory.hpp"
#include "Strings.hpp"
#include "Trace.hpp"
#include "Pe/ExportedFunctionsIterator.hpp"
#include "Pe/ImportedFunctionsIterator.hpp"
#include "Pe/ImportedModulesIterator.hpp"
#include "Pe/Pe.hpp"
#include "Pe/RelocationBlocksIterator.hpp"
#include "Pe/RelocationEntriesIterator.hpp"
#include "Pe/SectionIterator.hpp"

#include <Windows.h>

static uint32_t get_image_size_safe(const uint8_t* unloaded_module)
{
	const IMAGE_OPTIONAL_HEADER32* const optional_header = Pe::get_optional_header(unloaded_module);
	if (optional_header == nullptr)
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
		nullptr,
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

	if (!map_sections(unloaded_module))
	{
		result = ApricotCode::FAILED_PE_MAP_SECTIONS;
		return;
	}

	if (!perform_relocations())
	{
		result = ApricotCode::FAILED_PE_PERFORM_RELOCATIONS;
		return;
	}

	if (!load_dependencies())
	{
		result = ApricotCode::FAILED_LOAD_DEPENDENCIES;
		return;
	}

	if (!initialize_tls())
	{
		result = ApricotCode::FAILED_PE_INITIALIZE_TLS;
	}

	if (!finalize_sections())
	{
		result = ApricotCode::FAILED_PE_FINALIZE_SECTIONS;
		return;
	}

	BOOL entry_point_result = FALSE;
	if (!call_entry_point(DLL_PROCESS_ATTACH, entry_point_result))
	{
		result = ApricotCode::FAILED_PE_CALL_ENTRY_POINT;
		return;
	}
	if (entry_point_result == FALSE)
	{
		call_entry_point(DLL_PROCESS_DETACH, entry_point_result);
		result = ApricotCode::FAILED_ENTRY_POINT_FAILED;
		return;
	}

	m_is_initialized = true;
	result = ApricotCode::SUCCESS;
}

ApricotLibraryImpl::~ApricotLibraryImpl()
{
	if (!m_is_initialized)
	{
		return;
	}
	BOOL ignored_return_value = TRUE;
	if (!call_entry_point(DLL_PROCESS_DETACH, ignored_return_value))
	{
		TRACE(L"failed calling entry point on DETACH");
	}
	unload_dependencies();
}

void ApricotLibraryImpl::map_section_entry(const Pe::SectionIterator::Entry& entry)
{
	Shellcode::HeapMemory::memcpy(static_cast<uint8_t*>(m_memory.get()) + entry.rva, entry.raw_data, entry.raw_size);
}

bool ApricotLibraryImpl::map_sections(const void* module)
{
	Pe::SectionIterator section_iterator(module, m_is_initialized);
	if (!m_is_initialized)
	{
		return false;
	}
	while (section_iterator.has_next())
	{
		const Pe::SectionIterator::Entry entry = section_iterator.next();
		map_section_entry(entry);
	}
	return true;
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

bool ApricotLibraryImpl::finalize_sections()
{
	Pe::SectionIterator section_iterator(m_memory.get(), m_is_initialized);
	if (!m_is_initialized)
	{
		return false;
	}
	const IMAGE_OPTIONAL_HEADER32* optional_header = Pe::get_optional_header(m_memory.get());
	const uint32_t section_alignment = optional_header->SectionAlignment;
	while (section_iterator.has_next())
	{
		const Pe::SectionIterator::Entry entry = section_iterator.next();
		if (!finalize_section_entry(entry, section_alignment))
		{
			return false;
		}
	}
	return true;
}

bool ApricotLibraryImpl::perform_relocations()
{
	bool result = false;
	Pe::RelocationBlocksIterator relocations(m_memory.get(), result);
	if (!result)
	{
		return false;
	}
	const IMAGE_OPTIONAL_HEADER32* const optional_header = Pe::get_optional_header(m_memory.get());
	if (optional_header == nullptr)
	{
		return false;
	}
	const uint32_t relocation_delta = reinterpret_cast<uint32_t>(m_memory.get()) - optional_header->ImageBase;
	while (relocations.has_next())
	{
		const Pe::RelocationBlocksIterator::Entry relocation_block = relocations.next();
		Pe::RelocationEntriesIterator entries(relocation_block);
		while (entries.has_next())
		{
			const Pe::RelocationEntriesIterator::Entry entry = entries.next();
			if (entry.type == Pe::RelocationEntriesIterator::Type::REL_ABSOLUTE)
			{
				continue;
			}
			if (entry.type != Pe::RelocationEntriesIterator::Type::REL_HIGHLOW)
			{
				return false;
			}
			*const_cast<uint32_t*>(entry.address) += relocation_delta;
		}
	}
	return true;
}

bool ApricotLibraryImpl::initialize_tls()
{
}

bool ApricotLibraryImpl::call_entry_point(const DWORD reason, BOOL& return_value)
{
	const IMAGE_OPTIONAL_HEADER32* const optional_header = Pe::get_optional_header(m_memory.get());
	if (optional_header == nullptr)
	{
		return false;
	}
	auto entry_point = reinterpret_cast<DllEntryPoint>(static_cast<uint8_t*>(m_memory.get()) + optional_header->
		AddressOfEntryPoint);
	__try
	{
		static constexpr DWORD ENTRY_POINT_RESERVED = 0;
		return_value = entry_point(static_cast<HMODULE>(m_memory.get()), reason, ENTRY_POINT_RESERVED);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
	return true;
}

bool ApricotLibraryImpl::load_module(const IMAGE_IMPORT_DESCRIPTOR* module)
{
	auto name = static_cast<const char*>(m_memory.get()) + module->Name;
	const HMODULE loaded_module = LoadLibraryA(name);
	if (loaded_module == nullptr)
	{
		return false;
	}
	bool result = false;
	Pe::ImportedFunctionsIterator imported_functions(m_memory.get(), module, result);
	if (!result)
	{
		return false;
	}
	while (imported_functions.has_next())
	{
		const IMAGE_THUNK_DATA32* imported_function = imported_functions.next();
		LPCSTR function_identifier = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(static_cast<const uint8_t*>(m_memory.
				get()) +
			imported_function->u1.AddressOfData)->Name;
		if (IMAGE_SNAP_BY_ORDINAL32(imported_function->u1.Function))
		{
			function_identifier = MAKEINTRESOURCEA(imported_function->u1.Ordinal);
		}
		void* const resolved_address = GetProcAddress(loaded_module, function_identifier);
		if (resolved_address == nullptr)
		{
			return false;
		}
		const_cast<IMAGE_THUNK_DATA32*>(imported_function)->u1.Function = reinterpret_cast<DWORD>(resolved_address);
	}
	return true;
}

bool ApricotLibraryImpl::load_dependencies()
{
	Pe::ImportedModulesIterator iterator(m_memory.get(), m_is_initialized);
	if (!m_is_initialized)
	{
		return false;
	}
	for (uint32_t loaded_modules = 0; iterator.has_next(); loaded_modules++)
	{
		const IMAGE_IMPORT_DESCRIPTOR* const imported_module = iterator.next();
		if (!load_module(imported_module))
		{
			unload_dependencies(loaded_modules + 1);
			return m_is_initialized = false;
		}
	}
	return m_is_initialized = true;
}

void ApricotLibraryImpl::unload_dependency(const IMAGE_IMPORT_DESCRIPTOR* module)
{
	auto name = static_cast<const char*>(m_memory.get()) + module->Name;
	const HMODULE h_module = GetModuleHandleA(name);
	if (h_module == nullptr)
	{
		TRACE(L"failed to get module handle");
		return;
	}
	if (FreeLibrary(h_module) == FALSE)
	{
		TRACE(L"failed to free library");
	}
}

void ApricotLibraryImpl::unload_dependencies(const uint32_t loaded_modules)
{
	bool result = false;
	Pe::ImportedModulesIterator iterator(m_memory.get(), result);
	if (!result)
	{
		TRACE(L"failed to parse IAT on unload");
	}
	for (uint32_t i = 0; i < loaded_modules && iterator.has_next(); ++i)
	{
		const IMAGE_IMPORT_DESCRIPTOR* const imported_module = iterator.next();
		unload_dependency(imported_module);
	}
}

void ApricotLibraryImpl::unload_dependencies()
{
	static constexpr uint32_t UNLOAD_ALL = 0xFFFFFFFF;
	return unload_dependencies(UNLOAD_ALL);
}

ApricotCode ApricotLibraryImpl::get_proc_address(const uint16_t ordinal, void*& result) const
{
	bool parse_result = false;
	Pe::ExportedFunctionsIterator exported_functions(m_memory.get(), parse_result);
	if (!parse_result)
	{
		return ApricotCode::FAILED_PE_PARSE_EAT;
	}
	Pe::ExportedFunctionsIterator::Entry function{};
	while (exported_functions.has_next())
	{
		if (!exported_functions.next(function))
		{
			return ApricotCode::FAILED_PE_PARSE_EAT;
		}
		if (function.ordinal == ordinal)
		{
			result = static_cast<uint8_t*>(m_memory.get()) + function.rva;
			return ApricotCode::SUCCESS;
		}
	}
	return ApricotCode::FAILED_ORDINAL_NOT_FOUND;
}

ApricotCode ApricotLibraryImpl::get_proc_address(const char* name, void*& result) const
{
	bool parse_result = false;
	Pe::ExportedFunctionsIterator exported_functions(m_memory.get(), parse_result);
	if (!parse_result)
	{
		return ApricotCode::FAILED_PE_PARSE_EAT;
	}
	Pe::ExportedFunctionsIterator::Entry function{};
	while (exported_functions.has_next())
	{
		if (!exported_functions.next(function))
		{
			return ApricotCode::FAILED_PE_PARSE_EAT;
		}
		if (function.name == nullptr)
		{
			continue;
		}
		if (Strings::equals(name, function.name))
		{
			result = static_cast<uint8_t*>(m_memory.get()) + function.rva;
			return ApricotCode::SUCCESS;
		}
	}
	return ApricotCode::FAILED_NAME_NOT_FOUND;
}
