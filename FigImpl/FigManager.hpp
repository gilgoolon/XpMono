#pragma once
#include "FigApi.hpp"
#include "IOperationHandler.hpp"
#include "Synchronization/CriticalSection.hpp"
#include "Synchronization/Event.hpp"
#include "Synchronization/UnmanagedEvent.hpp"

#include <unordered_map>

namespace FigManager
{
// implement the following when creating a fig with this interface
extern Fig::FigInformation g_information;
extern std::wstring g_name;
[[nodiscard]] extern std::shared_ptr<IOperationHandler>
make_handler(Fig::OperationType operation_type,
             const Buffer& operation_parameters,
             std::unique_ptr<Event> operation_event);
// up to here

extern std::shared_ptr<UnmanagedEvent> g_quit_event;
extern std::unordered_map<Fig::OperationId, std::shared_ptr<IOperationHandler>> g_operations;
extern std::unique_ptr<CriticalSection> g_operations_lock;

[[nodiscard]] Fig::FigCode __cdecl initialize(__in HANDLE unmanaged_quit_event,
                                              __out Fig::FigInterfaces* interfaces,
                                              __out Fig::FigInformation* information);

[[nodiscard]] Fig::FigCode __cdecl execute(__in Fig::OperationType operation,
                                           __in const uint8_t* parameters_buffer,
                                           __in uint32_t parameters_buffer_size,
                                           __out Fig::OperationId* id,
                                           __out HANDLE* operation_event);

[[nodiscard]] Fig::FigCode __cdecl status(__in Fig::OperationId id,
                                          __out Fig::ExecutionStatus* status,
                                          __out Fig::FigSpecificCode* specific_code);

[[nodiscard]] Fig::FigCode __cdecl take(__in Fig::OperationId id,
                                        __out uint8_t* buffer,
                                        __inout uint32_t* buffer_size);
};
