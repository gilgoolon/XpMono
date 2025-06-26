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

	struct ImportedFunction final
	{
		const IMAGE_THUNK_DATA32* unbound;
		const IMAGE_THUNK_DATA32* bound;
	};

	[[nodiscard]] bool has_next() const;
	[[nodiscard]] ImportedFunction next();

private:
	const IMAGE_THUNK_DATA32* m_next_unbound_thunk;
	const IMAGE_THUNK_DATA32* m_next_bound_thunk;
};
}
