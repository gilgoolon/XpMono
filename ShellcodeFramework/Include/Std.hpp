#pragma once
#include <cstdint>

void* operator new(uint32_t size);

void operator delete(void* ptr) noexcept;

struct View final
{
	void* address;
	size_t size;
};
