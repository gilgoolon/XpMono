#include "Registry/RegistryManager.hpp"

#include "Registry/RegistryKey.hpp"

RegistryManager::RegistryValue RegistryManager::get_key_value(
	const RegistryPaths::FullPath& registry_path,
	const std::wstring& name,
	const RegistryValueFlag& value_type
)
{
	const RegistryKey registry_key(registry_path);

	return registry_key.get_value(name, value_type);
}

void RegistryManager::set_key_value(
	const RegistryPaths::FullPath& registry_path,
	const std::wstring& name,
	const BYTE* value,
	const RegistryValueType& value_type
)
{
	const RegistryKey registry_key(registry_path);

	registry_key.set_value(name, value, value_type);
}
