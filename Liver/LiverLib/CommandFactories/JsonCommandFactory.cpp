#include "CommandFactories/JsonCommandFactory.hpp"

#include "Json.hpp"
#include "Trace.hpp"
#include "Commands/CallDllGenericProcedureCommand.hpp"
#include "Commands/CallDllProcedureCommand.hpp"
#include "Commands/ExecuteFigOperationCommand.hpp"
#include "Commands/LoadDllCommand.hpp"
#include "Commands/LoadFigCommand.hpp"
#include "Commands/UnloadDllCommand.hpp"
#include "Commands/UnloadFigCommand.hpp"
#include "Crypto/Base64.hpp"
#include "Utils/Strings.hpp"

namespace Params
{
// Info section
static constexpr auto INFO_SECTION = "info";
static constexpr auto COMMAND_TYPE = "type";

// Parameters section
static constexpr auto PARAMETERS_SECTION = "parameters";

namespace LoadDll
{
static constexpr auto LIBRARY_BUFFER = "library_buffer";
static constexpr auto LIBRARY_ID = "library_id";
}

namespace UnloadDll
{
static constexpr auto LIBRARY_ID = "library_id";
}

namespace CallDllProcedure
{
static constexpr auto LIBRARY_ID = "library_id";
static constexpr auto ORDINAL = "ordinal";
static constexpr auto PARAMETERS = "parameters";
}

namespace LoadFig
{
static constexpr auto FIG_BUFFER = "fig_buffer";
}

namespace UnloadFig
{
static constexpr auto FIG_ID = "fig_id";
}

namespace ExecuteFigOperation
{
static constexpr auto FIG_ID = "fig_id";
static constexpr auto OPERATION_PARAMETERS = "parameters";
static constexpr auto OPERATION_TYPE = "operation_type";
}
}

ICommand::Ptr JsonCommandFactory::create(const Command& command)
{
	const Json data = Json::parse(Strings::to_string(command.data));
	const Json& info = data[Params::INFO_SECTION];
	const ICommand::Type command_type = info[Params::COMMAND_TYPE].get<ICommand::Type>();
	const Json& parameters = data[Params::PARAMETERS_SECTION];

	switch (command_type)
	{
	case ICommand::Type::LOAD_DLL:
	{
		TRACE(data.dump().data());
		const std::string library_buffer = parameters[Params::LoadDll::LIBRARY_BUFFER];
		const auto library_id = parameters[Params::LoadDll::LIBRARY_ID].get<LibrariesContainer::LibraryId>();
		return std::make_shared<LoadDllCommand>(command.id, library_id, Base64::decode(library_buffer));
	}

	case ICommand::Type::UNLOAD_DLL:
	{
		const auto library_id = parameters[Params::UnloadDll::LIBRARY_ID].get<LibrariesContainer::LibraryId>();
		return std::make_shared<UnloadDllCommand>(command.id, library_id);
	}

	case ICommand::Type::CALL_DLL_PROCEDURE:
	{
		const auto library_id = parameters[Params::CallDllProcedure::LIBRARY_ID].get<uint32_t>();
		const auto ordinal = parameters[Params::CallDllProcedure::ORDINAL].get<uint16_t>();
		return std::make_shared<CallDllProcedureCommand>(command.id, library_id, ordinal);
	}

	case ICommand::Type::CALL_DLL_GENERIC_PROCEDURE:
	{
		const auto library_id = parameters[Params::CallDllProcedure::LIBRARY_ID].get<uint32_t>();
		const auto ordinal = parameters[Params::CallDllProcedure::ORDINAL].get<uint16_t>();
		const auto encoded_parameters = parameters[Params::CallDllProcedure::PARAMETERS].get<std::string>();
		return std::make_shared<CallDllGenericProcedureCommand>(
			command.id,
			library_id,
			ordinal,
			Base64::decode(encoded_parameters)
		);
	}

	case ICommand::Type::LOAD_FIG:
	{
		const std::string library_buffer = parameters[Params::LoadFig::FIG_BUFFER];
		return std::make_shared<LoadFigCommand>(command.id, Base64::decode(library_buffer));
	}

	case ICommand::Type::UNLOAD_FIG:
	{
		const auto fig_id = parameters[Params::UnloadFig::FIG_ID].get<Fig::FigId>();
		return std::make_shared<UnloadFigCommand>(command.id, fig_id);
	}

	case ICommand::Type::EXECUTE_FIG_OPERATION:
	{
		const auto fig_id = parameters[Params::ExecuteFigOperation::FIG_ID].get<Fig::FigId>();
		const auto operation_type = parameters[Params::ExecuteFigOperation::OPERATION_TYPE].get<Fig::OperationType>();
		const auto encoded_operation_parameters = parameters[Params::ExecuteFigOperation::OPERATION_PARAMETERS].get<
			std::string>();
		return std::make_shared<ExecuteFigOperationCommand>(
			command.id,
			fig_id,
			operation_type,
			Base64::decode(encoded_operation_parameters)
		);
	}

	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}
