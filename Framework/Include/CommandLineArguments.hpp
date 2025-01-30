#pragma once
#include <cstdint>
#include <string>
#include <vector>

class CommandLineArguments final
{
public:
	explicit CommandLineArguments(uint32_t size);
	~CommandLineArguments() = default;
	CommandLineArguments(const CommandLineArguments&) = delete;
	CommandLineArguments& operator=(const CommandLineArguments&) = delete;
	CommandLineArguments(CommandLineArguments&&) = delete;
	CommandLineArguments& operator=(CommandLineArguments&&) = delete;

	[[nodiscard]] std::wstring wstring(uint32_t idx) const;
	[[nodiscard]] std::uint32_t uint32(uint32_t idx) const;

private:
	std::vector<std::wstring> m_arguments;

	[[nodiscard]] static std::vector<std::wstring> retrieve_command_line();
};
