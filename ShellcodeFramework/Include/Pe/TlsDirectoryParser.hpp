#pragma once
#include "Std.hpp"

#include <Windows.h>

namespace Pe
{
using Callback = BOOL(*APIENTRY)(HINSTANCE, DWORD, LPVOID);

struct TlsDirectory final
{
	View data;
	uint32_t* index_address;
	Callback* callbacks;
	uint32_t zeros_padding;
};

[[nodiscard]] bool parse_tls_directory(const void* module, TlsDirectory& tls_directory);
}
