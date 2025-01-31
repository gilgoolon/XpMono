#pragma once
#include <Windows.h>

namespace Pe
{
class ImportedModulesIterator final
{
public:
	explicit ImportedModulesIterator(const void* module, bool& result);
	~ImportedModulesIterator() = default;
	ImportedModulesIterator(const ImportedModulesIterator&) = delete;
	ImportedModulesIterator& operator=(const ImportedModulesIterator&) = delete;
	ImportedModulesIterator(ImportedModulesIterator&&) = delete;
	ImportedModulesIterator& operator=(ImportedModulesIterator&&) = delete;

	[[nodiscard]] bool has_next() const;

	[[nodiscard]] const IMAGE_IMPORT_DESCRIPTOR* next();

private:
	const IMAGE_IMPORT_DESCRIPTOR* m_next_module;
};
}
