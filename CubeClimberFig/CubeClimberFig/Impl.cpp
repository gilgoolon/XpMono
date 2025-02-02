#include "FigApi.hpp"
#include "Synchronization/UnmanagedEvent.hpp"

#include <type_traits>

static constexpr Fig::FigId FIG_ID = 1;
static constexpr Fig::VersionMajor VERSION_MAJOR = 6;
static constexpr Fig::VersionMinor VERSION_MINOR = 9;

static std::unique_ptr<UnmanagedEvent> g__quit_event = nullptr;

Fig::FigCode __cdecl execute([[maybe_unused]] __in Fig::OperationType operation,
                             [[maybe_unused]] __in const uint8_t* parameters_buffer,
                             [[maybe_unused]] __in uint32_t parameters_buffer_size,
                             [[maybe_unused]] __out Fig::OperationId* id,
                             [[maybe_unused]] __out HANDLE* operation_event)
{
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
	return Fig::FigCode::SUCCESS;
}

static_assert(std::is_same_v<decltype(initialize), decltype(Fig::initialize)>, "wrong initialize definition");
static_assert(std::is_same_v<decltype(execute), decltype(Fig::execute)>, "wrong execute definition");
static_assert(std::is_same_v<decltype(status), decltype(Fig::status)>, "wrong status definition");
static_assert(std::is_same_v<decltype(take), decltype(Fig::take)>, "wrong take definition");
