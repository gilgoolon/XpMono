#include "Api.hpp"
#include "Config.hpp"
#include "FigApi.hpp"
#include "HandlerRunner.hpp"
#include "IOperationHandler.hpp"
#include "Operations/DirlistHandler.hpp"
#include "Processes/Thread.hpp"
#include "Synchronization/CriticalSection.hpp"
#include "Synchronization/Event.hpp"
#include "Synchronization/UnmanagedEvent.hpp"
#include "Utils/Random.hpp"

#include <unordered_map>

static constexpr Fig::FigId FIG_ID = 1;
static constexpr Fig::VersionMajor VERSION_MAJOR = 6;
static constexpr Fig::VersionMinor VERSION_MINOR = 9;

static std::shared_ptr<UnmanagedEvent> g__quit_event = nullptr;
static std::unordered_map<Fig::OperationId, std::shared_ptr<IOperationHandler>> g__operations;
static std::unique_ptr<CriticalSection> g__operations_lock = nullptr;

Fig::FigCode __cdecl execute([[maybe_unused]] __in const Fig::OperationType operation,
                             [[maybe_unused]] __in const uint8_t* const parameters_buffer,
                             [[maybe_unused]] __in const uint32_t parameters_buffer_size,
                             [[maybe_unused]] __out Fig::OperationId* const id,
                             [[maybe_unused]] __out HANDLE* const operation_event)
{
	const uint32_t generated_id = Random::uint32();
	*id = generated_id;
	auto event = std::make_unique<Event>(
		std::wstring{Config::EVENT_PREFIX} + std::to_wstring(generated_id),
		Event::Type::AUTO_RESET
	);
	*operation_event = event->handle();
	std::shared_ptr<IOperationHandler> handler = nullptr;
	switch (operation)
	{
	case static_cast<Fig::OperationType>(CubeClimberOperation::DIRLIST):
		handler = std::make_shared<DirlistHandler>(std::move(event));
		break;
	default:
		return Fig::FigCode::FAILED_UNSUPPORTED_OPERATION;
	}
	Thread handler_worker(std::make_unique<HandlerRunner>(handler));
	const CriticalSection::Acquired acquired = g__operations_lock->acquire();
	g__operations.emplace(
		generated_id,
		std::move(handler)
	);
	return Fig::FigCode::SUCCESS;
}

Fig::FigCode __cdecl status([[maybe_unused]] __in const Fig::OperationId id,
                            [[maybe_unused]] __out Fig::ExecutionStatus* const status,
                            [[maybe_unused]] __out Fig::FigSpecificCode* const specific_code)
{
	const CriticalSection::Acquired acquired = g__operations_lock->acquire();
	const auto found = g__operations.find(id);
	if (found == g__operations.end())
	{
		return Fig::FigCode::FAILED_INVALID_OPERATION_ID;
	}
	const std::shared_ptr<IOperationHandler>& handler = found->second;
	*status = handler->status();
	*specific_code = handler->specific_code();
	return Fig::FigCode::SUCCESS;
}

Fig::FigCode __cdecl take([[maybe_unused]] __in const Fig::OperationId id,
                          [[maybe_unused]] __out uint8_t* const buffer,
                          [[maybe_unused]] __inout uint32_t* const buffer_size)
{
	const CriticalSection::Acquired acquired = g__operations_lock->acquire();
	const auto found = g__operations.find(id);
	if (found == g__operations.end())
	{
		return Fig::FigCode::FAILED_INVALID_OPERATION_ID;
	}
	const std::shared_ptr<IOperationHandler>& handler = found->second;
	const Buffer result = handler->take(*buffer_size);
	*buffer_size = result.size();
	std::copy_n(result.data(), result.size(), buffer);
	return Fig::FigCode::SUCCESS;
}

Fig::FigCode __cdecl initialize(__in HANDLE unmanaged_quit_event,
                                __out Fig::FigInterfaces* interfaces,
                                __out Fig::FigInformation* information)
{
	*information = {FIG_ID, VERSION_MAJOR, VERSION_MINOR};
	*interfaces = {execute, status, take};
	g__quit_event = std::make_shared<UnmanagedEvent>(unmanaged_quit_event);
	g__operations_lock = std::make_unique<CriticalSection>();
	return Fig::FigCode::SUCCESS;
}

static_assert(std::is_same_v<decltype(initialize), decltype(Fig::initialize)>, "wrong initialize definition");
static_assert(std::is_same_v<decltype(execute), decltype(Fig::execute)>, "wrong execute definition");
static_assert(std::is_same_v<decltype(status), decltype(Fig::status)>, "wrong status definition");
static_assert(std::is_same_v<decltype(take), decltype(Fig::take)>, "wrong take definition");
