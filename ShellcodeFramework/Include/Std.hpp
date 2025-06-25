#pragma once
#include <cstdint>
#include <type_traits>

void* operator new(size_t size);
void* operator new(size_t size, void* address);

void operator delete(void* ptr) noexcept;

struct View final
{
	void* address;
	size_t size;
};
