#pragma once
#include "Std.hpp"

#include <Windows.h>

namespace Pe
{
using TlsCallback = BOOL (APIENTRY*)(HINSTANCE, DWORD, LPVOID);

struct TlsDirectory final
{
	View data;
	uint32_t* index_address;
	TlsCallback* callbacks;
	uint32_t zeros_padding;
};

[[nodiscard]] bool parse_tls_directory(const void* module, TlsDirectory& tls_directory);
}
