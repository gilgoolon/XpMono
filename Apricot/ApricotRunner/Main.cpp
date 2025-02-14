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
	catch (const ApricotException& ex)
	{
		TRACE(
			"uncaught ApricotException with code ",
			ex.code(),
			" and apricot_code ",
			static_cast<uint32_t>(ex.apricot_code())
		)
	}
	catch (const WinApiException& ex)
	{
		TRACE("uncaught WinApiException with code ", ex.code(), " and error ", ex.error())
	}
	catch (const Exception& ex)
	{
		TRACE("uncaught Exception with code ", ex.code())
	}
	catch (const std::exception& ex)
	{
		TRACE("uncaught std::exception: ", ex.what())
	}
	catch (...)
	{
		TRACE("uncaught unknown or critical exception")
	}
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
	static constexpr std::string_view BY_NAME = "exported_function_by_name";
	library.call<EmptyFunction>(BY_ORDINAL);
	library.call<EmptyFunction>(std::string{BY_NAME});
}
