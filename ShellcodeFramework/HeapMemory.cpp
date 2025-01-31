#include "HeapMemory.hpp"

#include "Trace.hpp"

Shellcode::HeapMemory::HeapMemory(const uint32_t size, const Permissions permissions, bool& result):
	m_address(nullptr),
	m_size(size),
	m_is_initialized(false)
{
	static constexpr void* NO_PREFERRED_ADDRESS = nullptr;
	m_is_initialized = allocate_memory(NO_PREFERRED_ADDRESS, size, permissions, m_address);
	result = m_is_initialized;
}

Shellcode::HeapMemory::HeapMemory(void* preferred_address,
                                  const uint32_t size,
                                  const Permissions permissions,
                                  bool& result):
	m_address(nullptr),
	m_size(size),
	m_is_initialized(false)
{
	m_is_initialized = allocate_memory(preferred_address, size, permissions, m_address);
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
