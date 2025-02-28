#include "Exception.hpp"
#include "LiverApi.hpp"
#include "Trace.hpp"
#include "Filesystem/File.hpp"
#include "Protections/ProgramProtector.hpp"

#include <Windows.h>

static constexpr auto CONFIG_PATH = L"../liver.json";

int WINAPI wWinMain([[maybe_unused]] HINSTANCE,
                    [[maybe_unused]] HINSTANCE,
                    [[maybe_unused]] PWSTR,
                    [[maybe_unused]] int)
{
	try
	{
		Protections::ProgramProtector protection;
		const Buffer configuration_buffer = File(CONFIG_PATH, File::Mode::READ, File::Disposition::OPEN).read();
		LiverApi::main(configuration_buffer);
		return EXIT_SUCCESS;
	}
	CATCH_AND_TRACE()
	return EXIT_FAILURE;
}
