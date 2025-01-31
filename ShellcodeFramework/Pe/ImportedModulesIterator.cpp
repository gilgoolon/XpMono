#include "Pe/ImportedModulesIterator.hpp"

#include "Pe/Pe.hpp"

Pe::ImportedModulesIterator::ImportedModulesIterator(const void* module, bool& result) :
	m_next_module(nullptr)
{
	result = false;
	const IMAGE_DATA_DIRECTORY* const import_data_directory = get_data_directory(
		module,
		IMAGE_DIRECTORY_ENTRY_IMPORT
	);

	if (import_data_directory == nullptr)
	{
		return;
	}
	m_next_module = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(static_cast<const uint8_t*>(module) +
		import_data_directory->VirtualAddress);

	result = true;
}

bool Pe::ImportedModulesIterator::has_next() const
{
	static constexpr DWORD CHARACTERISTICS_TERMINATOR = 0;
	return m_next_module->Characteristics != CHARACTERISTICS_TERMINATOR;
}

const IMAGE_IMPORT_DESCRIPTOR* Pe::ImportedModulesIterator::next()
{
	const IMAGE_IMPORT_DESCRIPTOR* result = m_next_module;
	m_next_module++;
	return result;
}
