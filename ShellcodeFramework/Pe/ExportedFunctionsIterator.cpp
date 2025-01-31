#include "Pe/ExportedFunctionsIterator.hpp"

#include "Pe/Pe.hpp"

ExportedFunctionsIterator::ExportedFunctionsIterator(const void* module, bool& result):
	m_module(module),
	m_num_functions(0),
	m_functions(nullptr),
	m_num_names(0),
	m_names(nullptr),
	m_name_ordinals(nullptr),
	m_base(0),
	m_current_index(0)
{
	const IMAGE_DATA_DIRECTORY* const export_data_directory = Pe::get_data_directory(
		module,
		IMAGE_DIRECTORY_ENTRY_EXPORT
	);
	if (export_data_directory == nullptr)
	{
		result = false;
		return;
	}
	const auto export_directory = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(
		static_cast<const uint8_t*>(module) + export_data_directory->VirtualAddress);
	m_num_functions = export_directory->NumberOfFunctions;
	m_functions = reinterpret_cast<const uint32_t*>(static_cast<const uint8_t*>(module) + export_directory->
		AddressOfFunctions);
	m_num_names = export_directory->NumberOfNames;
	m_names = reinterpret_cast<const uint32_t*>(static_cast<const uint8_t*>(module) + export_directory->AddressOfNames);
	m_name_ordinals = reinterpret_cast<const uint32_t*>(static_cast<const uint8_t*>(module) + export_directory->
		AddressOfNameOrdinals);
	m_base = export_directory->Base;
	result = true;
}

const char* ExportedFunctionsIterator::get_function_name(const uint16_t ordinal) const
{
	for (uint32_t i = 0; i < m_num_names; ++i)
	{
		if (m_name_ordinals[i] == ordinal)
		{
			return static_cast<const char*>(m_module) + m_names[i];
		}
	}
	return nullptr;
}

bool ExportedFunctionsIterator::has_next() const
{
	while (m_current_index < m_num_functions && m_functions[m_current_index] == 0)
	{
		m_current_index++;
	}
	return m_current_index < m_num_functions;
}

bool ExportedFunctionsIterator::next(Entry& result)
{
	if (!has_next())
	{
		return false;
	}
	const uint16_t ordinal = static_cast<uint16_t>(m_base + m_current_index);
	const char* const name = get_function_name(ordinal);
	result = {m_functions[m_current_index], ordinal, name};
	++m_current_index;
	return true;
}
