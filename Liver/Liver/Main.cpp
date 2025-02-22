#include "LiverApi.hpp"
#include "Trace.hpp"
#include "Protections/ProgramProtector.hpp"

#include <Windows.h>

int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance,
                    [[maybe_unused]] HINSTANCE hPrevInstance,
                    [[maybe_unused]] PWSTR pCmdLine,
                    [[maybe_unused]] int nCmdShow)
{
	try
	{
		Protections::ProgramProtector protection;
		LiverApi::main({});
		return EXIT_SUCCESS;
	}
	catch (...)
	{
		TRACE("uncaught unknown or critical exception");
	}
	return EXIT_SUCCESS;
}
