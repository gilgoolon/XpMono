#pragma once
#include "ComReleaser.hpp"

#include <optional>
#include <string>
#include <wbemcli.h>

class ComResult final
{
public:
	explicit ComResult(IWbemClassObject* object);
	~ComResult() = default;
	ComResult(const ComResult&) = delete;
	ComResult& operator=(const ComResult&) = delete;
	ComResult(ComResult&&) = delete;
	ComResult& operator=(ComResult&&) = delete;

	[[nodiscard]] std::optional<std::wstring> get_formatted_property(const std::wstring& property_name);

private:
	ComReleaser m_object;
};
