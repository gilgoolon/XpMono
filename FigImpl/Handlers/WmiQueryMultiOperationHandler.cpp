#include "Handlers/WmiQueryMultiOperationHandler.hpp"

#include "Com/ComConnection.hpp"
#include "Products/TextTypedProduct.hpp"
#include "Utils/Strings.hpp"

WmiQueryMultiOperationHandler::WmiQueryMultiOperationHandler(std::unique_ptr<Event> operation_event,
                                                             std::wstring class_name,
                                                             std::vector<std::wstring> fields):
	IOperationHandler(std::move(operation_event)),
	m_class_name(std::move(class_name)),
	m_fields(std::move(fields))
{
}

void WmiQueryMultiOperationHandler::run()
{
	const ComConnection connection;
	const std::wstring query = L"SELECT * FROM " + m_class_name;
	static constexpr auto PAIR_SUFFIX = L"\n";
	static constexpr auto FIELD_VALUE_SEPARATOR = L": ";
	static constexpr auto UNKNOWN_VALUE = L"?";
	static constexpr auto FIELD_PREFIX = L"- ";
	uint32_t i = 1;

	std::wstring product;
	for (const std::unique_ptr<ComResult>& entry : connection.query(query))
	{
		product.append(m_class_name + std::to_wstring(i++) + PAIR_SUFFIX);
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
			product.append(formatted_result);
		}
	}
	append(std::make_unique<TextTypedProduct>(product));
	finished();
}
