#pragma once
#include <Windows.h>

class ImportedFunctionIterator final
{
public:
	explicit ImportedFunctionIterator(const void* module,
	                                  const IMAGE_IMPORT_DESCRIPTOR* imported_module,
	                                  bool loaded,
	                                  bool& result);
	~ImportedFunctionIterator() = default;
	ImportedFunctionIterator(const ImportedFunctionIterator&) = delete;
	ImportedFunctionIterator& operator=(const ImportedFunctionIterator&) = delete;
	ImportedFunctionIterator(ImportedFunctionIterator&&) = delete;
	ImportedFunctionIterator& operator=(ImportedFunctionIterator&&) = delete;

	[[nodiscard]] bool has_next() const;
	[[nodiscard]] void* next();

private:
	const IMAGE_THUNK_DATA32* m_next_thunk;
	const void* m_module;
	bool m_loaded;
};
