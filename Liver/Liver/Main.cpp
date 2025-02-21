#include "ApricotException.hpp"
#include "Exception.hpp"
#include "FigException.hpp"
#include "Liver.hpp"
#include "Trace.hpp"

#include <Windows.h>

int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance,
                    [[maybe_unused]] HINSTANCE hPrevInstance,
                    [[maybe_unused]] PWSTR pCmdLine,
                    [[maybe_unused]] int nCmdShow)
{
	try
	{
		Liver::main({});
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
