#include "Communicators/Protocol/ExecuteCommandsResponse.hpp"

ExecuteCommandsResponse::ExecuteCommandsResponse(std::vector<Buffer> commands):
	m_commands(std::move(commands))
{
}

IResponse::Type ExecuteCommandsResponse::type() const
{
	return Type::EXECUTE_COMMANDS;
}

const std::vector<Buffer>& ExecuteCommandsResponse::commands() const
{
	return m_commands;
}
