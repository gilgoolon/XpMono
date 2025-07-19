#pragma once
#include <string>
#include <windows.h>

namespace RegistryPaths
{
struct FullPath
{
	HKEY root;
	std::wstring subkey;
};

namespace Microsoft
{
using RegistryPaths::FullPath;

inline const FullPath CLIPBOARD(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Clipboard");
}
}
