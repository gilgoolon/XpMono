#include "CredentialsGrabber.hpp"

#include "Utils/Formatting.hpp"
#include "Utils/Strings.hpp"

static std::optional<std::wstring> format_datetime(const std::optional<Time::Datetime>& datetime)
{
	if (!datetime.has_value())
	{
		return {};
	}

	return Strings::to_wstring(Time::to_string(datetime.value()));
}

std::wstring Credential::serialize() const
{
	const Formatting::Fields fields = {
		{L"origin", origin},
		{L"username", username},
		{L"password", password},
		{L"created_time", format_datetime(created_time)},
		{L"accessed_time", format_datetime(accessed_time)},
		{L"modified_time", format_datetime(modified_time)},
	};

	return Formatting::format_fields(fields);
}
