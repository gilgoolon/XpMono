#pragma once
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

	[[nodiscard]] std::wstring value() const;

private:
	VARIANT m_variant;
};
