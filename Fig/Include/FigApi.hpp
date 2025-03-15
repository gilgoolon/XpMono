#pragma once
#include <cstdint>
#include <Windows.h>

namespace Fig
{
enum class ExecutionStatus : uint32_t
{
	FAILED = 0,
	EXECUTING,
	EXECUTING_CAN_TAKE,
	FINISHED,
};

enum class FigCode : uint32_t
{
	SUCCESS = 0,
	FAILED_UNKNOWN,
	FAILED_RUN,
	FAILED_UNSUPPORTED_OPERATION,
	FAILED_INVALID_OPERATION_ID,
};

using OperationType = uint32_t;
using OperationId = uint32_t;
using FigSpecificCode = uint32_t;
static constexpr FigSpecificCode FIG_SPECIFIC_CODE_RESERVED = 0;

struct FigInterfaces;
struct FigInformation;

FigCode __cdecl initialize(__in HANDLE unmanaged_quit_event,
                           __out FigInterfaces* interfaces,
                           __out FigInformation* information);

FigCode __cdecl execute(__in OperationType operation,
                        __in const uint8_t* parameters_buffer,
                        __in uint32_t parameters_buffer_size,
                        __out OperationId* id,
                        __out HANDLE* operation_event);

FigCode __cdecl status(__in OperationId id, __out ExecutionStatus* status, __out FigSpecificCode* specific_code);

FigCode __cdecl take(__in OperationId id, __out uint8_t* buffer, __inout uint32_t* buffer_size);

using InitializeFunction = decltype(initialize)*;
using ExecuteFunction = decltype(execute)*;
using StatusFunction = decltype(status)*;
using TakeFunction = decltype(take)*;

struct FigInterfaces final
{
	ExecuteFunction execute;
	StatusFunction status;
	TakeFunction take;
};

using FigId = uint32_t;
using VersionMajor = uint16_t;
using VersionMinor = uint16_t;

struct FigInformation final
{
	FigId fig_id;
	VersionMajor version_major;
	VersionMinor version_minor;
};

static constexpr uint16_t INITIALIZE_ORDINAL = 1;
}
