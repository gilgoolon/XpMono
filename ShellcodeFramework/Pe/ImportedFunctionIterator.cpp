#include "Pe/ImportedFunctionIterator.hpp"

#include <cstdint>

ImportedFunctionIterator::ImportedFunctionIterator(const void* module,
                                                   const IMAGE_IMPORT_DESCRIPTOR* imported_module,
                                                   const bool loaded,
                                                   bool& result):
	m_next_thunk(nullptr),
	m_module(module),
	m_loaded(loaded)
{
	result = false;
	m_next_thunk = reinterpret_cast<const IMAGE_THUNK_DATA32*>(static_cast<const uint8_t*>(module) +
		imported_module->FirstThunk);
	result = true;
}

bool ImportedFunctionIterator::has_next() const
{
	static constexpr DWORD TERMINATOR = 0;
	return m_next_thunk->u1.Function != TERMINATOR;
}

void* ImportedFunctionIterator::next()
{
	const IMAGE_THUNK_DATA32* const result = m_next_thunk;
	++m_next_thunk;
	if (m_loaded)
	{
		return reinterpret_cast<void*>(result->u1.Function);
	}
	if (IMAGE_SNAP_BY_ORDINAL32(result->u1.Function))
	{
		return MAKEINTRESOURCEA(result->u1.Ordinal);
	}
	const auto import_by_name = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(static_cast<const uint8_t*>(m_module) +
		result->u1.AddressOfData);
	return const_cast<LPSTR>(import_by_name->Name);
}
