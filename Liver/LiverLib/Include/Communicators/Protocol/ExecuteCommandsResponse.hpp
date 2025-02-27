#pragma once
#include "Command.hpp"
#include "IResponse.hpp"
#include "Utils/Buffer.hpp"

class ExecuteCommandsResponse final : public IResponse
{
public:
	explicit ExecuteCommandsResponse(std::vector<Command> commands);
	~ExecuteCommandsResponse() override = default;
	ExecuteCommandsResponse(const ExecuteCommandsResponse&) = delete;
	ExecuteCommandsResponse& operator=(const ExecuteCommandsResponse&) = delete;
	ExecuteCommandsResponse(ExecuteCommandsResponse&&) = delete;
	ExecuteCommandsResponse& operator=(ExecuteCommandsResponse&&) = delete;

	[[nodiscard]] Type type() const override;

	[[nodiscard]] const std::vector<Command>& commands() const;

private:
	std::vector<Command> m_commands;
};
