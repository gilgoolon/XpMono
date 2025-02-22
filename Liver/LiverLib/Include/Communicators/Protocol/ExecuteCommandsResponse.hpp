#pragma once
#include "IResponse.hpp"
#include "Utils/Buffer.hpp"

class ExecuteCommandsResponse final : public IResponse
{
public:
	explicit ExecuteCommandsResponse(std::vector<Buffer> commands);
	~ExecuteCommandsResponse() override = default;
	ExecuteCommandsResponse(const ExecuteCommandsResponse&) = delete;
	ExecuteCommandsResponse& operator=(const ExecuteCommandsResponse&) = delete;
	ExecuteCommandsResponse(ExecuteCommandsResponse&&) = delete;
	ExecuteCommandsResponse& operator=(ExecuteCommandsResponse&&) = delete;

	[[nodiscard]] Type type() const override;

	[[nodiscard]] const std::vector<Buffer>& commands() const;

private:
	std::vector<Buffer> m_commands;
};
