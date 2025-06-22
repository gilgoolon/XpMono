#include "Handlers/WmiQuerySingleOperationHandler.hpp"

#include "Com/Connection.hpp"
#include "Products/TextTypedProduct.hpp"
#include "Utils/Strings.hpp"

WmiQuerySingleOperationHandler::WmiQuerySingleOperationHandler(std::unique_ptr<Event> operation_event,
                                                               const std::wstring& class_name,
                                                               std::vector<std::wstring>&& fields):
	IOperationHandler(std::move(operation_event)),
	m_class_name(class_name),
	m_fields(std::move(fields))
{
}

void WmiQuerySingleOperationHandler::run()
{
	const Com::Connection connection;
	const std::wstring query = L"SELECT * FROM " + m_class_name;
	const std::vector<std::unique_ptr<Com::Result>> results = connection.query(query);
	Com::Result& os = *results.front();
	static constexpr auto PAIR_SUFFIX = L"\n";
	static constexpr auto FIELD_VALUE_SEPARATOR = L": ";
	static constexpr auto UNKNOWN_VALUE = L"?";

	std::wstring product;
	for (const std::wstring& field : m_fields)
	{
		const std::optional<std::wstring> result = os.get_formatted_property(field);
		const std::wstring value = result.has_value() ? result.value() : UNKNOWN_VALUE;
		std::wstring formatted_result = Strings::concat(
			field,
			std::wstring{FIELD_VALUE_SEPARATOR},
			value,
			std::wstring{PAIR_SUFFIX}
		);
		product.append(formatted_result);
	}
	append(std::make_unique<TextTypedProduct>(std::move(product)));
	finished();
}
