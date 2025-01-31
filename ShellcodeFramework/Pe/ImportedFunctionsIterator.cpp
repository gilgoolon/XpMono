#include "Pe/ImportedFunctionsIterator.hpp"

#include <cstdint>

Pe::ImportedFunctionsIterator::ImportedFunctionsIterator(const void* module,
                                                         const IMAGE_IMPORT_DESCRIPTOR* imported_module,
                                                         bool& result):
	m_next_thunk(nullptr),
	m_module(module)
{
	result = false;
	m_next_thunk = reinterpret_cast<const IMAGE_THUNK_DATA32*>(static_cast<const uint8_t*>(module) +
		imported_module->FirstThunk);
	result = true;
}

bool Pe::ImportedFunctionsIterator::has_next() const
{
	static constexpr DWORD TERMINATOR = 0;
	return m_next_thunk->u1.Function != TERMINATOR;
}

const IMAGE_THUNK_DATA32* Pe::ImportedFunctionsIterator::next()
{
	return m_next_thunk++;
}
