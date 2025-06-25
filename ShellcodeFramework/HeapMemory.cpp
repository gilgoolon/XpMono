#include "HeapMemory.hpp"

#include "Std.hpp"
#include "Trace.hpp"

#include <utility>

static constexpr void* NO_PREFERRED_ADDRESS = nullptr;

Shellcode::HeapMemory::HeapMemory(const uint32_t size, const Permissions permissions, bool& result):
	HeapMemory(NO_PREFERRED_ADDRESS, size, permissions, result)
{
}

Shellcode::HeapMemory::HeapMemory(void* preferred_address,
                                  const uint32_t size,
                                  const Permissions permissions,
                                  bool& result):
	m_address(nullptr),
	m_is_initialized(allocate_memory(preferred_address, size, permissions, m_address))
{
	result = m_is_initialized;
}

Shellcode::HeapMemory::~HeapMemory()
{
	if (!m_is_initialized)
	{
		return;
	}
	static constexpr DWORD FREE_ALL_SIZE = 0;
	if (VirtualFree(m_address, FREE_ALL_SIZE, MEM_RELEASE) == FALSE)
	{
		TRACE(L"failed to free heap memory");
	}
}

Shellcode::HeapMemory::HeapMemory(HeapMemory&& other) noexcept:
	m_address(std::exchange(other.m_address, nullptr)),
	m_is_initialized(std::exchange(other.m_is_initialized, false))
{
}

Shellcode::HeapMemory& Shellcode::HeapMemory::operator=(HeapMemory&& other) noexcept
{
	if (this != &other)
	{
		HeapMemory temp(std::move(other));
		swap(temp);
	}
	return *this;
}

bool Shellcode::HeapMemory::allocate_memory(void* const preferred_address,
                                            const uint32_t size,
                                            Permissions permissions,
                                            void*& result)
{
	static constexpr DWORD COMMIT_MEMORY = MEM_COMMIT | MEM_RESERVE;
	result = VirtualAlloc(preferred_address, size, COMMIT_MEMORY, static_cast<DWORD>(permissions));
	if (result == nullptr)
	{
		TRACE(L"failed to allocate heap memory");
		return false;
	}
	return true;
}

void* Shellcode::HeapMemory::get() const
{
	return m_address;
}

void Shellcode::HeapMemory::memcpy(void* dest, const void* source, const uint32_t size)
{
	auto cur_dest = static_cast<uint8_t*>(dest);
	auto cur_source = static_cast<const uint8_t*>(source);
	for (uint32_t i = 0; i < size; ++i)
	{
		*cur_dest++ = *cur_source++;
	}
}

uint32_t Shellcode::HeapMemory::align(const uint32_t offset, const uint32_t alignment)
{
	const uint32_t rem = offset % alignment;
	if (rem == 0)
	{
		return offset;
	}
	return offset - rem + alignment;
}

bool Shellcode::HeapMemory::set_permissions(void* address, const uint32_t size, const DWORD perms)
{
	DWORD old_perms = 0;
	return VirtualProtect(address, size, perms, &old_perms) != FALSE;
}

void Shellcode::HeapMemory::swap(HeapMemory& other)
	noexcept
{
	void* const temp_address = m_address;
	const bool temp_is_initialized = m_is_initialized;
	m_address = other.m_address;
	m_is_initialized = other.m_is_initialized;
	other.m_address = temp_address;
	other.m_is_initialized = temp_is_initialized;
}
