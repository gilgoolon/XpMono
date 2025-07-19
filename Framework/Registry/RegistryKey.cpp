#include "Registry/RegistryKey.hpp"

#include <vector>

RegistryKey::RegistryKey(const RegistryPaths::FullPath& registry_path) :
	m_handle(open(registry_path))
{
}

RegistryKey::~RegistryKey()
{
	close();
}

HKEY RegistryKey::open(const RegistryPaths::FullPath& registry_path)
{
	HKEY handle;

	if (RegOpenKeyExW(registry_path.root, registry_path.subkey.c_str(), 0, KEY_ALL_ACCESS, &handle) != ERROR_SUCCESS)
	{
		throw;
		//TODO: Throw an error
	}

	return handle;
}

void RegistryKey::close() const
{
	RegCloseKey(m_handle);
}

RegistryManager::RegistryValue RegistryKey::get_value(
	const std::wstring& name,
	const RegistryValueFlag& value_flag
) const
{
	std::vector<BYTE> buffer;
	DWORD size;

	// First call to get size of target value
	if (RegGetValueW(
		m_handle,
		nullptr,
		name.c_str(),
		static_cast<DWORD>(value_flag),
		nullptr,
		nullptr,
		&size
	) != ERROR_SUCCESS)
	{
		throw;
		//TODO: Throw an error
	}

	// Resize buffer to hold the value
	buffer.resize(size);

	// Second call to get the actual data
	if (RegGetValueW(
		m_handle,
		nullptr,
		name.c_str(),
		static_cast<DWORD>(value_flag),
		nullptr,
		buffer.data(),
		&size
	) != ERROR_SUCCESS)
	{
		throw;
		// TODO: throw or handle error
	}

	// Interpreting the value
	switch (value_flag)
	{
	case RegistryValueFlag::UINT32:
	{
		DWORD int_value = *reinterpret_cast<DWORD*>(buffer.data());
		return int_value;
	}
	default:
		throw;
	}
}

void RegistryKey::set_value(
	const std::wstring& name,
	const BYTE* value,
	const RegistryValueType& value_type
) const
{
	if (RegSetValueExW(
		m_handle,
		name.c_str(),
		0,
		static_cast<DWORD>(value_type),
		value,
		sizeof(value)
	) != ERROR_SUCCESS)
	{
		//TODO: Throw an error
	}
}
