#pragma once
#include "RegistryPaths.hpp"
#include "RegistryValueType.hpp"
#include <string>
#include <variant>

namespace RegistryManager
{
using RegistryValue = std::variant<uint32_t>;

[[nodiscard]] RegistryValue get_key_value(
	const RegistryPaths::FullPath& registry_path,
	const std::wstring& name,
	const RegistryValueFlag& value_type
);
void set_key_value(
	const RegistryPaths::FullPath& registry_path,
	const std::wstring& name,
	const BYTE* value,
	const RegistryValueType& value_type
);
};
