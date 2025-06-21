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

Credential::Credential(std::wstring origin,
                       std::wstring username,
                       std::wstring password,
                       std::optional<Time::Datetime> created_time,
                       std::optional<Time::Datetime> accessed_time,
                       std::optional<Time::Datetime> modified_time):
	origin(std::move(origin)),
	username(std::move(username)),
	password(std::move(password)),
	created_time(std::move(created_time)),
	accessed_time(std::move(accessed_time)),
	modified_time(std::move(modified_time))
{
}

std::wstring Credential::type() const
{
	return L"Credential";
}

ISerializableStruct::Fields Credential::fields() const
{
	return {
		{L"origin", origin},
		{L"username", username},
		{L"password", password},
		{L"created_time", format_datetime(created_time)},
		{L"accessed_time", format_datetime(accessed_time)},
		{L"modified_time", format_datetime(modified_time)},
	};
}

std::wstring NotSupported::type() const
{
	return L"Not Supported";
}

ISerializableStruct::Fields NotSupported::fields() const
{
	return {};
}
