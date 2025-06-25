#pragma once
#include "Std.hpp"

template <typename T>
class Optional
{
public:
	Optional() :
		m_has_value(false)
	{
	}

	Optional(const Optional&) = delete;
	Optional& operator=(const Optional&) = delete;
	Optional(Optional&&) = delete;
	Optional& operator=(Optional&&) = delete;

	~Optional()
	{
		reset();
	}

	template <typename... Args>
	void emplace(Args&&... args)
	{
		reset();
		new(get_storage()) T(std::forward<Args>(args)...);
		m_has_value = true;
	}

	void reset()
	{
		if (m_has_value)
		{
			get()->~T();
			m_has_value = false;
		}
	}

	[[nodiscard]] bool has_value() const
	{
		return m_has_value;
	}

	T* operator->()
	{
		return get();
	}

	const T* operator->() const
	{
		return get();
	}

	T& operator*()
	{
		return *get();
	}

	const T& operator*() const
	{
		return *get();
	}

	T* get()
	{
		return reinterpret_cast<T*>(&m_storage);
	}

	const T* get() const
	{
		return reinterpret_cast<const T*>(&m_storage);
	}

	explicit operator bool() const
	{
		return m_has_value;
	}

private:
	void* get_storage()
	{
		return static_cast<void*>(&m_storage);
	}

	union Storage
	{
		char raw[sizeof(T)];
		long long align;
	} m_storage;

	bool m_has_value;
};
