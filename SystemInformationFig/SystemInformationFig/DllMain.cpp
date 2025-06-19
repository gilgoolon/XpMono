#include "Api.hpp"
#include "Exception.hpp"
#include "FigImplException.hpp"
#include "FigManager.hpp"
#include "Trace.hpp"
#include "Operations/BiosInformationHandler.hpp"
#include "Operations/OsInformationHandler.hpp"
#include "Operations/PhysicalDrivesInformationHandler.hpp"
#include "Operations/UsersInformationHandler.hpp"
#include "Protections/LibraryProtector.hpp"
#include "Operations/NetworkInformationHandler.hpp"

#include <Windows.h>

Fig::FigInformation FigManager::g_information = {Api::FIG_ID, Api::VERSION_MAJOR, Api::VERSION_MINOR};
std::wstring FigManager::g_name =
#ifdef _DEBUG
	L"SystemInformationFig";
#else
	L"{0FDD56F8-3517-41D2-9100-DE02774B4EF9}";
#endif

std::shared_ptr<IOperationHandler> FigManager::make_handler(const Fig::OperationType operation_type,
                                                            [[maybe_unused]] const Buffer& operation_parameters,
                                                            std::unique_ptr<Event> operation_event)
{
	switch (operation_type)
	{
	case static_cast<Fig::OperationType>(Api::OperationType::OS_INFO):
		return std::make_shared<OsInformationHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(Api::OperationType::USERS_INFO):
		return std::make_shared<UsersInformationHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(Api::OperationType::PHYSICAL_DRIVES_INFO):
		return std::make_shared<PhysicalDrivesInformationHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(Api::OperationType::BIOS_INFO):
		return std::make_shared<BiosInformationHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(Api::OperationType::NETWORK_INFO):
		return std::make_shared<NetworkInformationHandler>(std::move(operation_event));
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
