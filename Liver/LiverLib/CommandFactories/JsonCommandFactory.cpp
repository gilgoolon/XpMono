#include "CommandFactories/JsonCommandFactory.hpp"

#include "Json.hpp"
#include "Commands/LoadDllCommand.hpp"
#include "Crypto/Base64.hpp"
#include "Utils/Strings.hpp"

namespace Params
{
// Info section
static constexpr std::string_view INFO_SECTION = "info";
static constexpr std::string_view COMMAND_TYPE = "type";

// Parameters section
static constexpr std::string_view PARAMETERS_SECTION = "parameters";

namespace LoadDll
{
static constexpr std::string_view LIBRARY_BUFFER = "library_buffer";
static constexpr std::string_view LIBRARY_ID = "library_id";
}
}

ICommand::Ptr JsonCommandFactory::create(const Buffer& command)
{
	const Json data = Json::from_bson(Strings::to_string(command));
	const Json info = data[std::string{Params::INFO_SECTION}];
	const ICommand::Type command_type = info[std::string{Params::COMMAND_TYPE}].get<ICommand::Type>();
	const Json parameters = data[std::string{Params::PARAMETERS_SECTION}];

	switch (command_type)
	{
	case ICommand::Type::LOAD_DLL:
	{
		const std::string library_buffer = parameters[std::string{Params::LoadDll::LIBRARY_BUFFER}];
		const auto library_id = parameters[std::string{Params::LoadDll::LIBRARY_ID}]
			.get<LibrariesContainer::LibraryId>();
		return std::make_shared<LoadDllCommand>(library_id, Base64::decode(library_buffer));
	}
	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}
