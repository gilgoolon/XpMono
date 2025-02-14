#pragma once
#include "ILibrary.hpp"

#include <filesystem>
#include <Windows.h>

class DynamicLibrary final : public ILibrary, std::enable_shared_from_this<DynamicLibrary>
{
public:
	explicit DynamicLibrary(const std::filesystem::path& dll_path);
	~DynamicLibrary() override;
	DynamicLibrary(const DynamicLibrary&) = delete;
	DynamicLibrary& operator=(const DynamicLibrary&) = delete;
	DynamicLibrary(DynamicLibrary&&) = delete;
	DynamicLibrary& operator=(DynamicLibrary&&) = delete;

private:
	[[nodiscard]] static HMODULE load_library(const std::filesystem::path& dll_path);

	[[nodiscard]] void* get_exported_procedure(const std::string& name) const override;
	[[nodiscard]] void* get_exported_procedure(uint16_t ordinal) const override;

	HMODULE m_module;
};
