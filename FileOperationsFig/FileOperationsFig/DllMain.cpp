#include "Exception.hpp"
#include "FigImplException.hpp"
#include "FigManager.hpp"
#include "Trace.hpp"
#include "Operations/DirlistHandler.hpp"
#include "Operations/SystemInformationHandler.hpp"
#include "Protections/LibraryProtector.hpp"

#include <Windows.h>

Fig::FigInformation FigManager::g_information = {1, 6, 9};
std::wstring FigManager::g_name = L"FileFetcher";

std::shared_ptr<IOperationHandler> FigManager::make_handler(const Fig::OperationType operation_type,
                                                            [[maybe_unused]] const Buffer& operation_parameters,
                                                            std::unique_ptr<Event> operation_event)
{
	switch (operation_type)
	{
	case static_cast<Fig::OperationType>(SystemInformationHandler::TYPE):
		return std::make_shared<SystemInformationHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(DirlistHandler::TYPE):
		return std::make_shared<DirlistHandler>(std::move(operation_event), operation_parameters);
	default:
		throw FigImplException(Fig::FigCode::FAILED_UNSUPPORTED_OPERATION);
	}
}

void process_attach()
{
	TRACE(L"PROCESS ATTACH")
}

void process_detach()
{
	TRACE(L"PROCESS DETACH")
}

static std::unique_ptr<Protections::LibraryProtector> g_protector = nullptr;

BOOL APIENTRY DllMain([[maybe_unused]] const HINSTANCE hInstance,
                      [[maybe_unused]] const DWORD dwReason,
                      [[maybe_unused]] const LPVOID lpReserved)
{
	try
	{
		if (dwReason == DLL_PROCESS_ATTACH)
		{
			g_protector = std::make_unique<Protections::LibraryProtector>();
			process_attach();
		}
		if (dwReason == DLL_PROCESS_DETACH)
		{
			process_detach();
			g_protector.reset();
		}
		return TRUE;
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
	return FALSE;
}
