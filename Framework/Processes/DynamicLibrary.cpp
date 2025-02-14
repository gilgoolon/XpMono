#include "Processes/DynamicLibrary.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

DynamicLibrary::DynamicLibrary(const std::filesystem::path& dll_path):
	m_module(load_library(dll_path))
{
}

DynamicLibrary::~DynamicLibrary()
{
	try
	{
		if (FreeLibrary(m_module) == FALSE)
		{
			TRACE(L"failed to free library")
		}
	}
	catch (...)
	{
		TRACE(L"failed to free library")
	}
}

HMODULE DynamicLibrary::load_library(const std::filesystem::path& dll_path)
{
	const HMODULE result = LoadLibraryW(dll_path.wstring().c_str());
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_LIBRARY_LOAD);
	}
	return result;
}

void* DynamicLibrary::get_exported_procedure(const std::string& name) const
{
	const FARPROC result = GetProcAddress(m_module, name.c_str());
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_LIBRARY_GET_PROC_ADDRESS);
	}
	return result;
}

void* DynamicLibrary::get_exported_procedure(const uint16_t ordinal) const
{
	const FARPROC result = GetProcAddress(m_module, MAKEINTRESOURCEA(ordinal));
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_LIBRARY_GET_PROC_ADDRESS);
	}
	return result;
}
