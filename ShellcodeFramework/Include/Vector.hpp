#pragma once

#include "Std.hpp"
#include "Trace.hpp"

template <typename T>
class Vector
{
	T* m_data;
	size_t m_size;
	size_t m_capacity;

	void* raw_allocate(const size_t bytes)
	{
		return operator new(bytes);
	}

	void raw_deallocate(void* ptr)
	{
		operator delete(ptr);
	}

	void grow()
	{
		const size_t new_capacity = m_capacity > 0 ? m_capacity * 2 : 4;
		void* new_block = raw_allocate(new_capacity * sizeof(T));
		if (!new_block)
		{
			TRACE(L"failed to grow vector");
			return;
		}

		T* new_data = static_cast<T*>(new_block);
		for (size_t i = 0; i < m_size; ++i)
		{
			new(new_data + i) T(std::move((m_data)[i]));
			(m_data + i)->~T();
		}

		raw_deallocate(m_data);
		m_data = new_data;
		m_capacity = new_capacity;
	}

public:
	explicit Vector() :
		m_data(nullptr),
		m_size(0),
		m_capacity(0)
	{
	}

	~Vector()
	{
		destroy();
	}

	void destroy()
	{
		for (size_t i = 0; i < m_size; ++i)
		{
			(m_data + i)->~T();
		}
		raw_deallocate(m_data);
		m_data = nullptr;
		m_size = 0;
		m_capacity = 0;
	}

	T& operator[](size_t index)
	{
		return m_data[index];
	}

	const T& operator[](size_t index) const
	{
		return m_data[index];
	}

	[[nodiscard]] size_t size() const
	{
		return m_size;
	}

	[[nodiscard]] size_t capacity() const
	{
		return m_capacity;
	}

	T* data()
	{
		return m_data;
	}

	const T* data() const
	{
		return m_data;
	}

	bool push_back(const T& value)
	{
		if (m_size == m_capacity) grow();
		if (m_size == m_capacity) return false;

		new(m_data + m_size) T(value);
		++m_size;
		return true;
	}

	template <typename... Args>
	bool emplace_back(Args&&... args)
	{
		if (m_size == m_capacity) grow();
		if (m_size == m_capacity) return false;

		new(m_data + m_size) T(std::forward<Args>(args)...);
		++m_size;
		return true;
	}
};
