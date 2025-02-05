#include "FigImplException.hpp"
#include "FigManager.hpp"
#include "Operations/DirlistHandler.hpp"
#include "Synchronization/Event.hpp"

Fig::FigInformation FigManager::g_information = {1, 6, 9};
std::wstring FigManager::g_name = L"CubeClimber";

using FigManager::initialize;

std::shared_ptr<IOperationHandler> FigManager::make_handler(const Fig::OperationType operation_type,
                                                            std::unique_ptr<Event> operation_event)
{
	switch (operation_type)
	{
	case static_cast<Fig::OperationType>(DirlistHandler::TYPE):
		return std::make_shared<DirlistHandler>(std::move(operation_event));
	default:
		throw FigImplException(Fig::FigCode::FAILED_UNSUPPORTED_OPERATION);
	}
}
