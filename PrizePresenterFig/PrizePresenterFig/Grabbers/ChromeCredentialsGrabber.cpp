#include "ChromeCredentialsGrabber.hpp"

std::optional<Credentials> ChromeCredentialsGrabber::grab_credentials() const
{
	return {};
}

std::wstring ChromeCredentialsGrabber::source() const
{
	return L"Chrome";
}
