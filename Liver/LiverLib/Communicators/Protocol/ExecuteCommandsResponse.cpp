#include "Communicators/Protocol/ExecuteCommandsResponse.hpp"

ExecuteCommandsResponse::ExecuteCommandsResponse(std::vector<Command> commands):
	m_commands(std::move(commands))
{
}

IResponse::Type ExecuteCommandsResponse::type() const
{
	return Type::EXECUTE_COMMANDS;
}

const std::vector<Command>& ExecuteCommandsResponse::commands() const
{
	return m_commands;
}
