#include "Api.hpp"
#include "Exception.hpp"
#include "FigImplException.hpp"
#include "FigManager.hpp"
#include "Trace.hpp"
#include "Operations/CheckStatusHandler.hpp"
#include "Operations/DisableHandler.hpp"
#include "Operations/EnableHandler.hpp"
#include "Operations/GetHistoryHandler.hpp"
#include "Protections/LibraryProtector.hpp"

#include <Windows.h>

Fig::FigInformation FigManager::g_information = {Api::FIG_ID, Api::VERSION_MAJOR, Api::VERSION_MINOR};
std::wstring FigManager::g_name =
#ifdef _DEBUG
	L"ClipboardInformationFig";
#else
	L"{5FA9DFB7-8F16-4733-80EF-D9E074F9E897}";
#endif

std::shared_ptr<IOperationHandler> FigManager::make_handler(const Fig::OperationType operation_type,
                                                            [[maybe_unused]] const Buffer& operation_parameters,
                                                            std::unique_ptr<Event> operation_event)
{
	switch (operation_type)
	{
	case static_cast<Fig::OperationType>(Api::OperationType::CHECK_CLIPBOARD_STATUS):
		return std::make_shared<CheckStatusHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(Api::OperationType::DISABLE_CLIPBOARD):
		return std::make_shared<DisableHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(Api::OperationType::ENABLE_CLIPBOARD):
		return std::make_shared<EnableHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(Api::OperationType::GET_HISTORY):
		return std::make_shared<GetHistoryHandler>(std::move(operation_event), operation_parameters);
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
