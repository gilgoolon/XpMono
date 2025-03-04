﻿#include "CommandFactories/JsonCommandFactory.hpp"

#include "Json.hpp"
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

namespace LoadFig
{
static constexpr auto FIG_BUFFER = "fig_buffer";
static constexpr auto FIG_ID = "fig_id";
}

namespace UnloadFig
{
static constexpr auto FIG_ID = "fig_id";
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
		const std::string library_buffer = parameters[Params::LoadDll::LIBRARY_BUFFER];
		const auto library_id = parameters[Params::LoadDll::LIBRARY_ID].get<LibrariesContainer::LibraryId>();
		return std::make_shared<LoadDllCommand>(command.id, library_id, Base64::decode(library_buffer));
	}

	case ICommand::Type::UNLOAD_DLL:
	{
		const auto library_id = parameters[Params::UnloadDll::LIBRARY_ID].get<LibrariesContainer::LibraryId>();
		return std::make_shared<UnloadDllCommand>(command.id, library_id);
	}

	case ICommand::Type::LOAD_FIG:
	{
		const std::string library_buffer = parameters[Params::LoadFig::FIG_BUFFER];
		const auto library_id = parameters[Params::LoadFig::FIG_ID].get<Fig::FigId>();
		return std::make_shared<LoadFigCommand>(command.id, library_id, Base64::decode(library_buffer));
	}

	case ICommand::Type::UNLOAD_FIG:
	{
		const auto fig_id = parameters[Params::UnloadFig::FIG_ID].get<Fig::FigId>();
		return std::make_shared<UnloadFigCommand>(command.id, fig_id);
	}

	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}
