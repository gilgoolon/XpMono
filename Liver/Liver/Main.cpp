#include "Exception.hpp"
#include "LiverApi.hpp"
#include "Trace.hpp"
#include "Protections/ProgramProtector.hpp"

#include <Windows.h>

int WINAPI wWinMain([[maybe_unused]] HINSTANCE,
                    [[maybe_unused]] HINSTANCE,
                    [[maybe_unused]] PWSTR,
                    [[maybe_unused]] int)
{
	try
	{
		Protections::ProgramProtector protection;
		LiverApi::main({});
		return EXIT_SUCCESS;
	}
	CATCH_AND_TRACE()
	return EXIT_FAILURE;
}
