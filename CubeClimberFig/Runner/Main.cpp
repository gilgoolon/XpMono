#include "Exception.hpp"
#include "FigException.hpp"
#include "FigModule.hpp"
#include "Trace.hpp"
#include "Filesystem/File.hpp"

#include <ApricotException.hpp>

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
	static constexpr Fig::FigId FIG_ID = 1;
	const Buffer fig_buffer = File(L"../Debug/CubeClimberFig.dll", File::Mode::READ).read();
	FigModule fig(FIG_ID, fig_buffer);
	TRACE("fig id: ", fig.id(), " fig version: ", fig.major(), ".", fig.minor());
	TRACE("finished successfully")
}
