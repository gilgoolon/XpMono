#include "Std.hpp"

#include "Trace.hpp"

void* operator new(const size_t size)
{
	void* ptr = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!ptr)
	{
		TRACE(L"failed new");
		return nullptr;
	}
	return ptr;
}

void* operator new([[maybe_unused]] size_t size, void* address)
{
	return address;
}

void operator delete(void* ptr) noexcept
{
	if (ptr == nullptr)
	{
		return;
	}
	static constexpr DWORD RELEASE_ALL_SIZE = 0;
	if (VirtualFree(ptr, RELEASE_ALL_SIZE, MEM_RELEASE) == FALSE)
	{
		TRACE(L"failed delete");
	}
}
