#pragma once
#include "Releaser.hpp"

#include <optional>
#include <string>
#include <wbemcli.h>

namespace Com
{
class Result final
{
public:
	explicit Result(IWbemClassObject* object);
	~Result() = default;
	Result(const Result&) = delete;
	Result& operator=(const Result&) = delete;
	Result(Result&&) = delete;
	Result& operator=(Result&&) = delete;

	[[nodiscard]] std::optional<std::wstring> get_formatted_property(const std::wstring& property_name);

private:
	Releaser m_object;
};
}
