#include "Handlers/WmiQueryMultiOperationHandler.hpp"

#include "Utils/Strings.hpp"
#include "Wmi/WmiConnection.hpp"

WmiQueryMultiOperationHandler::WmiQueryMultiOperationHandler(std::unique_ptr<Event> operation_event,
                                                             const std::wstring& class_name,
                                                             std::vector<std::wstring>&& fields):
	IOperationHandler(std::move(operation_event)),
	m_class_name(class_name),
	m_fields(fields)
{
}

void WmiQueryMultiOperationHandler::run()
{
	const WmiConnection connection;
	const std::wstring query = L"SELECT * FROM " + m_class_name;
	static constexpr auto PAIR_SUFFIX = L"\n";
	static constexpr auto FIELD_VALUE_SEPARATOR = L": ";
	static constexpr auto UNKNOWN_VALUE = L"?";
	static constexpr auto FIELD_PREFIX = L"- ";
	uint32_t i = 1;
	for (const std::unique_ptr<WmiResult>& entry : connection.query(query))
	{
		append(Strings::to_buffer(m_class_name + std::to_wstring(i++) + PAIR_SUFFIX));
		for (const std::wstring& field : m_fields)
		{
			const std::optional<std::wstring> result = entry->get_formatted_property(field);
			const std::wstring value = result.has_value() ? result.value() : UNKNOWN_VALUE;
			std::wstring formatted_result = Strings::concat(
				std::wstring{FIELD_PREFIX},
				field,
				std::wstring{FIELD_VALUE_SEPARATOR},
				value,
				std::wstring{PAIR_SUFFIX}
			);
			const Buffer data = Strings::to_buffer(formatted_result);
			append(data);
		}
	}
	finished();
}
