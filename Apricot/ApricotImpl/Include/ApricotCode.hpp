#pragma once
#include <cstdint>

enum class ApricotCode : uint32_t
{
	SUCCESS = 0,
	UNKNOWN_ERROR,
	FAILED_MEM_ALLOC,
	FAILED_PE_PARSE_SECTIONS,
};
