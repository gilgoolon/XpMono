#pragma once
#include <Windows.h>

namespace Pe
{
class ImportedFunctionsIterator final
{
public:
	explicit ImportedFunctionsIterator(const void* module,
	                                   const IMAGE_IMPORT_DESCRIPTOR* imported_module,
	                                   bool& result);
	~ImportedFunctionsIterator() = default;
	ImportedFunctionsIterator(const ImportedFunctionsIterator&) = delete;
	ImportedFunctionsIterator& operator=(const ImportedFunctionsIterator&) = delete;
	ImportedFunctionsIterator(ImportedFunctionsIterator&&) = delete;
	ImportedFunctionsIterator& operator=(ImportedFunctionsIterator&&) = delete;

	[[nodiscard]] bool has_next() const;
	[[nodiscard]] const IMAGE_THUNK_DATA32* next();

private:
	const IMAGE_THUNK_DATA32* m_next_thunk;
	const void* m_module;
};
}
