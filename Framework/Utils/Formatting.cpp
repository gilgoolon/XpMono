#include "Utils/Formatting.hpp"

#include "Utils/Strings.hpp"

std::wstring Formatting::format_fields(const std::vector<std::pair<std::wstring, std::optional<std::wstring>>>& fields)
{
	static constexpr auto FIELD_SUFFIX = L"\n";
	static constexpr auto FIELD_VALUE_SEPARATOR = L": ";
	static constexpr auto UNKNOWN_VALUE = L"";
	static constexpr auto FIELD_PREFIX = L"- ";

	std::wstring result;

	for (const auto& [name, value] : fields)
	{
		const std::wstring resolved_value = value.has_value() ? value.value() : UNKNOWN_VALUE;
		std::wstring field = Strings::concat(
			std::wstring{FIELD_PREFIX},
			name,
			std::wstring{FIELD_VALUE_SEPARATOR},
			resolved_value,
			std::wstring{FIELD_SUFFIX}
		);
		result += field;
	}

	return result;
}
