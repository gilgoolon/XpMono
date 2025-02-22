#include "Include/LiverApi.hpp"

#include "Exception.hpp"
#include "Liver.hpp"
#include "Trace.hpp"

void LiverApi::main(const Buffer& configuration)
{
	try
	{
		const std::unique_ptr<Liver> liver = Liver::create(configuration);
		liver->run();
	}
	catch ([[maybe_unused]] const WinApiException& ex)
	{
		TRACE("uncaught WinApiException with code ", ex.code(), " and error ", ex.error())
	}
	catch ([[maybe_unused]] const Exception& ex)
	{
		TRACE("uncaught Exception with code ", ex.code())
	}
	catch ([[maybe_unused]] const CriticalException&)
	{
		TRACE("uncaught CriticalException")
	}
	catch ([[maybe_unused]] const std::exception& ex)
	{
		TRACE("uncaught std::exception: ", ex.what())
	}
	catch (...)
	{
		TRACE("uncaught unknown or critical exception")
	}
}
