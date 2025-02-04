#include "FigApi.hpp"
#include "Synchronization/CriticalSection.hpp"
#include "Synchronization/Event.hpp"
#include "Synchronization/UnmanagedEvent.hpp"

#include <unordered_map>

static constexpr Fig::FigId FIG_ID = 1;
static constexpr Fig::VersionMajor VERSION_MAJOR = 6;
static constexpr Fig::VersionMinor VERSION_MINOR = 9;

static std::unique_ptr<UnmanagedEvent> g__quit_event = nullptr;

struct OperationMetadata final
{
	std::unique_ptr<Event> operation_event;
};

static std::unordered_map<Fig::OperationId, OperationMetadata> g__operations;
static std::unique_ptr<CriticalSection> g__operations_lock = nullptr;

Fig::FigCode __cdecl execute([[maybe_unused]] __in Fig::OperationType operation,
                             [[maybe_unused]] __in const uint8_t* parameters_buffer,
                             [[maybe_unused]] __in uint32_t parameters_buffer_size,
                             [[maybe_unused]] __out Fig::OperationId* id,
                             [[maybe_unused]] __out HANDLE* operation_event)
{
	static Fig::OperationId next_id = 0;
	const Fig::OperationId generated_id = ++next_id;
	*id = generated_id;
	static constexpr std::wstring_view EVENT_PREFIX = L"Global\\CubeClimberEvent";
	auto event = std::make_unique<Event>(
		std::wstring{EVENT_PREFIX} + std::to_wstring(generated_id),
		Event::Type::AUTO_RESET
	);
	*operation_event = event->handle();
	const CriticalSection::Acquired acquired = g__operations_lock->acquire();
	g__operations.emplace(
		generated_id,
		OperationMetadata{std::move(event)}
	);
	return Fig::FigCode::SUCCESS;
}

Fig::FigCode __cdecl status([[maybe_unused]] __in Fig::OperationId id,
                            [[maybe_unused]] __out Fig::ExecutionStatus* status,
                            [[maybe_unused]] __out Fig::FigSpecificCode* specific_code)
{
	return Fig::FigCode::SUCCESS;
}

Fig::FigCode __cdecl take([[maybe_unused]] __in Fig::OperationId id,
                          [[maybe_unused]] __out uint8_t* buffer,
                          [[maybe_unused]] __inout uint32_t* buffer_size)
{
	return Fig::FigCode::SUCCESS;
}

Fig::FigCode __cdecl initialize(__in HANDLE unmanaged_quit_event,
                                __out Fig::FigInterfaces* interfaces,
                                __out Fig::FigInformation* information)
{
	*information = {FIG_ID, VERSION_MAJOR, VERSION_MINOR};
	*interfaces = {execute, status, take};
	g__quit_event = std::make_unique<UnmanagedEvent>(unmanaged_quit_event);
	g__operations_lock = std::make_unique<CriticalSection>();
	return Fig::FigCode::SUCCESS;
}

static_assert(std::is_same_v<decltype(initialize), decltype(Fig::initialize)>, "wrong initialize definition");
static_assert(std::is_same_v<decltype(execute), decltype(Fig::execute)>, "wrong execute definition");
static_assert(std::is_same_v<decltype(status), decltype(Fig::status)>, "wrong status definition");
static_assert(std::is_same_v<decltype(take), decltype(Fig::take)>, "wrong take definition");
