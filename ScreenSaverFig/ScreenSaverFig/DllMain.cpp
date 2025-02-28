#include "Api.hpp"
#include "Exception.hpp"
#include "FigImplException.hpp"
#include "FigManager.hpp"
#include "Trace.hpp"
#include "Operations/ScreenshotHandler.hpp"
#include "Protections/LibraryProtector.hpp"

#include <Windows.h>

Fig::FigInformation FigManager::g_information = {Api::FIG_ID, Api::VERSION_MAJOR, Api::VERSION_MINOR};
std::wstring FigManager::g_name =
#ifdef _DEBUG
	L"ScreenSaverFig";
#else
	L"{D9E32855-1F7F-4D6C-BDE2-CEE9FE03772F}";
#endif

std::shared_ptr<IOperationHandler> FigManager::make_handler(const Fig::OperationType operation_type,
                                                            [[maybe_unused]] const Buffer& operation_parameters,
                                                            std::unique_ptr<Event> operation_event)
{
	switch (operation_type)
	{
	case static_cast<Fig::OperationType>(Api::OperationType::TAKE_SCREENSHOT):
		return std::make_shared<ScreenshotHandler>(std::move(operation_event));
	default:
		throw FigImplException(Fig::FigCode::FAILED_UNSUPPORTED_OPERATION);
	}
}

void process_attach()
{
	TRACE(L"PROCESS ATTACH");
}

void process_detach()
{
	TRACE(L"PROCESS DETACH");
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
	CATCH_AND_TRACE()
	
	return FALSE;
}
