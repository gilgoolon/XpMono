#include "ApricotException.hpp"
#include "ApricotLibrary.hpp"
#include "Exception.hpp"
#include "Trace.hpp"
#include "Filesystem/File.hpp"

#include <Windows.h>

static void main_logic();

int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance,
                    [[maybe_unused]] HINSTANCE hPrevInstance,
                    [[maybe_unused]] PWSTR pCmdLine,
                    [[maybe_unused]] int nCmdShow)
{
	try
	{
		main_logic();
	}
	catch ([[maybe_unused]] const ApricotException& ex)
	{
		TRACE(
			"uncaught ApricotException with code ",
			ex.code(),
			" and apricot_code ",
			static_cast<uint32_t>(ex.apricot_code())
		);
	}
	CATCH_AND_TRACE()
	return EXIT_SUCCESS;
}

using EmptyFunction = void(*)();

static void main_logic()
{
	const Buffer dll_data = File(
		LR"(C:\Users\alper\OneDrive\Documents\XpMono\Apricot\Debug\SimpleDll.dll)",
		File::Mode::READ,
		File::Disposition::OPEN
	).read();
	const ApricotLibrary library(dll_data);

	static constexpr uint16_t BY_ORDINAL = 1;
	static constexpr auto BY_NAME = "exported_function_by_name";
	library.call<EmptyFunction>(BY_ORDINAL);
	library.call<EmptyFunction>(BY_NAME);
}
