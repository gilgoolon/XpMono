#pragma once
#include <cstdint>
#include <Windows.h>

namespace Shellcode
{
class HeapMemory final
{
public:
	enum class Permissions : uint32_t
	{
		READ_WRITE = PAGE_READWRITE,
		READ_EXECUTE = PAGE_EXECUTE_READ,
		READ = PAGE_READONLY,
	};

	explicit HeapMemory(uint32_t size, Permissions permissions, bool& result);
	explicit HeapMemory(void* preferred_address, uint32_t size, Permissions permissions, bool& result);
	~HeapMemory();
	HeapMemory(const HeapMemory&) = delete;
	HeapMemory& operator=(const HeapMemory&) = delete;
	HeapMemory(HeapMemory&&) = delete;
	HeapMemory& operator=(HeapMemory&&) = delete;

private:
	[[nodiscard]] static bool allocate_memory(void* preferred_address,
	                                          uint32_t size,
	                                          Permissions permissions,
	                                          __out void*& result);

public:
	[[nodiscard]] void* get() const;

private:
	void* m_address;
	bool m_is_initialized;
};
}
