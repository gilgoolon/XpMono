#include "ApricotException.hpp"
#include "ApricotLibrary.hpp"
#include "Exception.hpp"
#include "FigException.hpp"
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
		return EXIT_SUCCESS;
	}
	catch (const ApricotException& ex)
	{
		TRACE(
			"uncaught ApricotException with code ",
			ex.code(),
			" and ApricotCode ",
			static_cast<uint32_t>(ex.apricot_code())
		)
	}
	catch (const FigException& ex)
	{
		TRACE(
			"uncaught FigException with code ",
			ex.code(),
			" and FigCode ",
			static_cast<uint32_t>(ex.fig_code()),
			" and FigSpecificCode ",
			ex.fig_specific_code()
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

static void main_logic()
{
	ApricotLibrary lib(
		File(LR"(C:\Users\alper\OneDrive\Documents\XpMono\Apricot\Debug\SimpleDll.dll)", File::Mode::READ).read()
	);
	const Buffer fig_buffer = File(
		LR"(C:\Users\alper\OneDrive\Documents\XpMono\Liver\Debug\CubeClimberFig.dll)",
		File::Mode::READ
	).read();
	constexpr Fig::FigId fig_id = 1;
	TRACE(L"loading fig library with id ", fig_id);
	ApricotLibrary fig_lib(fig_buffer);
	using SimpleFunction = void(*)();
	lib.call<SimpleFunction>(1);
}
