#pragma once
#include "RegistryManager.hpp"
#include "RegistryValueType.hpp"
#include <string>
#include <variant>
#include <windows.h>

class RegistryKey
{
public:
	explicit RegistryKey(const RegistryPaths::FullPath& registry_path);
	RegistryKey(const RegistryKey&) = delete;
	RegistryKey& operator=(const RegistryKey&) = delete;
	RegistryKey(RegistryKey&&) = delete;
	RegistryKey& operator=(RegistryKey&&) = delete;
	~RegistryKey();

	[[nodiscard]] RegistryManager::RegistryValue get_value(
		const std::wstring& name,
		const RegistryValueFlag& value_flag
	) const;
	void set_value(
		const std::wstring& name,
		const BYTE* value,
		const RegistryValueType& value_type
	) const;

private:
	[[nodiscard]] static HKEY open(const RegistryPaths::FullPath& registry_path);
	void close() const;

	HKEY m_handle;
};
