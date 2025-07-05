#pragma once
#include <windows.h>

enum class RegistryValueType : DWORD
{
	UINT32 = REG_DWORD,
};

enum class RegistryValueFlag : DWORD
{
	UINT32 = RRF_RT_REG_DWORD,
};
