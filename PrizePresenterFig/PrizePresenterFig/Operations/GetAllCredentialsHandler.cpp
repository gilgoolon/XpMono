#include "GetAllCredentialsHandler.hpp"

#include "Products/TextTypedProduct.hpp"
#include "Utils/Strings.hpp"

GetAllCredentialsHandler::GetAllCredentialsHandler(std::unique_ptr<Event> operation_event):
	IOperationHandler(std::move(operation_event))
{
}

void GetAllCredentialsHandler::run()
{
	static constexpr auto SUFFIX = L"\n";
	std::wstring product;
	while (iterator->has_next())
	{
		product.append(Strings::concat(Strings::to_wstring(file->serialize()), std::wstring_view{SUFFIX}));
	}
	append(std::make_unique<TextTypedProduct>(product));
	finished();
}
