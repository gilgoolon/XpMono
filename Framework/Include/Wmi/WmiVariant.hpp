#pragma once
#include "SafeArrayAccess.hpp"
#include "Utils/Time.hpp"

#include <string>
#include <WMIUtils.h>

class WmiVariant final
{
public:
	explicit WmiVariant();
	~WmiVariant();
	WmiVariant(const WmiVariant&) = delete;
	WmiVariant& operator=(const WmiVariant&) = delete;
	WmiVariant(WmiVariant&&) = delete;
	WmiVariant& operator=(WmiVariant&&) = delete;

	[[nodiscard]] VARIANT* get();
	[[nodiscard]] VARTYPE type() const;

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
		const SafeArrayAccess access(m_variant.parray);
		return access.value<T>();
	}

private:
	VARIANT m_variant;
};
