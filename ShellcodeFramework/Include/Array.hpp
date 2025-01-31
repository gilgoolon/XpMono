#pragma once
#include "HeapMemory.hpp"

#include <cstdint>

namespace Shellcode
{
template <typename T>
class Array final
{
public:
	explicit Array(const uint32_t size, bool& result) :
		m_memory(size * sizeof(T), HeapMemory::Permissions::READ_WRITE, result)
	{
	}

	~Array() = default;
	Array(const Array&) = delete;
	Array& operator=(const Array&) = delete;
	Array(Array&&) = delete;
	Array& operator=(Array&&) = delete;

	T& operator[](const uint32_t index)
	{
		return const_cast<T&>(const_cast<const Array*>(this)->operator[](index));
	}

	const T& operator[](const uint32_t index) const
	{
		return static_cast<const T*>(m_memory.get())[index];
	}

private:
	HeapMemory m_memory;
};
}
