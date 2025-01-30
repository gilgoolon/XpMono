#pragma once
#include <filesystem>
#include <Windows.h>

class DynamicLibrary final : std::enable_shared_from_this<DynamicLibrary>
{
public:
	explicit DynamicLibrary(const std::filesystem::path& dll_path);
	~DynamicLibrary();
	DynamicLibrary(const DynamicLibrary&) = delete;
	DynamicLibrary& operator=(const DynamicLibrary&) = delete;
	DynamicLibrary(DynamicLibrary&&) = delete;
	DynamicLibrary& operator=(DynamicLibrary&&) = delete;

private:
	[[nodiscard]] static HMODULE load_library(const std::filesystem::path& dll_path);

	[[nodiscard]] void* get_exported_procedure(const std::string& name) const;

public:
	template <typename FunctionType, typename... Args>
	auto call(const std::string& name, Args&&... args) const
	{
		auto func = reinterpret_cast<FunctionType>(get_exported_procedure(name));
		return (*func)(std::forward<Args>(args)...);
	}

private:
	HMODULE m_module;
};
