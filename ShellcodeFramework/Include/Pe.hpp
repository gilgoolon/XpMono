#pragma once
#include <cstdint>
#include <Windows.h>

namespace Pe
{
const IMAGE_DOS_HEADER* get_dos_header(const void* module);
const IMAGE_NT_HEADERS32* get_nt_headers(const void* module);
const IMAGE_FILE_HEADER* get_file_header(const void* module);
const IMAGE_OPTIONAL_HEADER32* get_optional_header(const void* module);
const IMAGE_DATA_DIRECTORY* get_data_directory(const void* module, uint32_t index);
const IMAGE_SECTION_HEADER* get_section_table(const void* module);
};
