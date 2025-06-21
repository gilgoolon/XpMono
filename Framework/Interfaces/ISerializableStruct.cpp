#include "Interfaces/ISerializableStruct.hpp"

#include "Utils/Strings.hpp"

std::wstring ISerializableStruct::serialize() const
{
	static constexpr auto SEPARATOR = L"\n";
	static constexpr auto OBJECT_TYPE_PREFIX = L"#";
	return Strings::concat(
		std::wstring{OBJECT_TYPE_PREFIX},
		type(),
		std::wstring{SEPARATOR},
		format_fields(fields())
	);
}

std::wstring ISerializableStruct::format_fields(const Fields& fields)
{
	static constexpr auto FIELD_SUFFIX = L"\n";
	static constexpr auto FIELD_VALUE_SEPARATOR = L": ";
	static constexpr auto UNKNOWN_VALUE = L"";
	static constexpr auto FIELD_PREFIX = L"- ";

	std::wstring result;

	for (const auto& [name, value] : fields)
	{
		const std::wstring resolved_value = value.has_value() ? value.value() : UNKNOWN_VALUE;
		const std::wstring field = Strings::concat(
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
