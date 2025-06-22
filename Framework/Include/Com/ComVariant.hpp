#pragma once
#include "ComSafeArrayAccess.hpp"
#include "Utils/Time.hpp"

#include <string>
#include <WMIUtils.h>

class ComVariant final
{
public:
	explicit ComVariant();
	~ComVariant();
	ComVariant(const ComVariant&) = delete;
	ComVariant& operator=(const ComVariant&) = delete;
	ComVariant(ComVariant&&) = delete;
	ComVariant& operator=(ComVariant&&) = delete;

	[[nodiscard]] VARIANT* get();
	[[nodiscard]] VARTYPE type() const;
	[[nodiscard]] bool has_value() const;

	[[nodiscard]] std::wstring wstring() const;
	[[nodiscard]] bool boolean() const;
	[[nodiscard]] uint64_t uint64() const;
	[[nodiscard]] uint32_t uint32() const;
	[[nodiscard]] uint16_t uint16() const;
	[[nodiscard]] uint8_t uint8() const;
	[[nodiscard]] int64_t int64() const;
	[[nodiscard]] int32_t int32() const;
	[[nodiscard]] int16_t int16() const;
	[[nodiscard]] int8_t int8() const;
	[[nodiscard]] Time::Datetime datetime() const;

	template <typename T>
	[[nodiscard]] std::vector<T> array() const
	{
		const ComSafeArrayAccess access(m_variant.parray);
		return access.value<T>();
	}

private:
	VARIANT m_variant;
};
