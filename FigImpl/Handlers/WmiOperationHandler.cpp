#include "Handlers/WmiOperationHandler.hpp"

#include "Utils/Strings.hpp"
#include "Wmi/WmiConnection.hpp"

WmiOperationHandler::WmiOperationHandler(std::unique_ptr<Event> operation_event,
                                         const std::wstring& class_name,
                                         std::vector<std::wstring>&& fields):
	IOperationHandler(std::move(operation_event)),
	m_class_name(class_name),
	m_fields(fields)
{
}

void WmiOperationHandler::run()
{
	const WmiConnection connection;
	const std::wstring query = L"SELECT * FROM " + m_class_name;
	const std::vector<std::unique_ptr<WmiResult>> results = connection.query(query);
	WmiResult& os = *results.front();
	static constexpr std::wstring_view PAIR_SUFFIX = L"\n";
	static constexpr std::wstring_view FIELD_VALUE_SEPARATOR = L": ";
	static constexpr std::wstring_view UNKNOWN_VALUE = L"?";
	for (const std::wstring& field : m_fields)
	{
		const std::optional<std::wstring> result = os.get_formatted_property(field);
		const std::wstring value = result.has_value() ? result.value() : std::wstring{UNKNOWN_VALUE};
		std::wstring formatted_result = Strings::concat(
			field,
			std::wstring{FIELD_VALUE_SEPARATOR},
			value,
			std::wstring{PAIR_SUFFIX}
		);
		const Buffer data = Strings::to_buffer(formatted_result);
		append(data);
	}
	finished();
}
