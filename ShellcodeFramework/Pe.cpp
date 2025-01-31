#include "Include/Pe.hpp"

#include <cstdint>

const IMAGE_DOS_HEADER* Pe::get_dos_header(const void* const module)
{
	const auto header = static_cast<const IMAGE_DOS_HEADER*>(module);
	if (header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return nullptr;
	}
	return header;
}

const IMAGE_NT_HEADERS32* Pe::get_nt_headers(const void* const module)
{
	const IMAGE_DOS_HEADER* dos_header = get_dos_header(module);
	if (dos_header == nullptr)
	{
		return nullptr;
	}
	const auto header = reinterpret_cast<const IMAGE_NT_HEADERS32*>(static_cast<const uint8_t*>(module) + dos_header->
		e_lfanew);
	if (header->Signature != IMAGE_NT_SIGNATURE)
	{
		return nullptr;
	}
	return header;
}

const IMAGE_FILE_HEADER* Pe::get_file_header(const void* const module)
{
	const IMAGE_NT_HEADERS32* nt_headers = get_nt_headers(module);
	if (nt_headers == nullptr)
	{
		return nullptr;
	}
	return &nt_headers->FileHeader;
}

const IMAGE_OPTIONAL_HEADER32* Pe::get_optional_header(const void* const module)
{
	const IMAGE_NT_HEADERS32* nt_headers = get_nt_headers(module);
	if (nt_headers == nullptr)
	{
		return nullptr;
	}
	const IMAGE_OPTIONAL_HEADER32* const header = &nt_headers->OptionalHeader;
	if (header->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
	{
		return nullptr;
	}
	return header;
}

const IMAGE_DATA_DIRECTORY* Pe::get_data_directory(const void* module, const uint32_t index)
{
	const IMAGE_OPTIONAL_HEADER32* const optional_header = get_optional_header(module);
	if (optional_header == nullptr)
	{
		return nullptr;
	}
	return &optional_header->DataDirectory[index];
}

const IMAGE_SECTION_HEADER* Pe::get_section_table(const void* module)
{
	const IMAGE_NT_HEADERS32* const nt_headers = get_nt_headers(module);
	if (nt_headers == nullptr)
	{
		return nullptr;
	}
	return IMAGE_FIRST_SECTION(nt_headers);
}
