#pragma once
#include "WmiReleaser.hpp"

#include <string>
#include <wbemcli.h>

class WmiResult final
{
public:
	explicit WmiResult(IWbemClassObject* object);
	~WmiResult() = default;
	WmiResult(const WmiResult&) = delete;
	WmiResult& operator=(const WmiResult&) = delete;
	WmiResult(WmiResult&&) = delete;
	WmiResult& operator=(WmiResult&&) = delete;

	[[nodiscard]] std::wstring get_formatted_property(const std::wstring& property_name);

private:
	WmiReleaser m_object;
};
