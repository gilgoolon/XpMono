#include "CommandLineArguments.hpp"

#include "Exception.hpp"
#include "Processes/ScopedLocalHeapAllocation.hpp"

#include <algorithm>

CommandLineArguments::CommandLineArguments(const uint32_t size):
	m_arguments(retrieve_command_line())
{
	if (m_arguments.size() != size)
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}
}

std::wstring CommandLineArguments::wstring(const uint32_t idx) const
{
	return m_arguments[idx];
}

std::uint32_t CommandLineArguments::uint32(const uint32_t idx) const
{
	const std::wstring value = m_arguments[idx];
	if (!std::ranges::all_of(value, isdigit))
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}
	return std::stoul(value);
}

std::vector<std::wstring> CommandLineArguments::retrieve_command_line()
{
	int argc = 0;
	LPWSTR* const argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argv == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_GET_COMMAND_LINE);
	}
	ScopedLocalHeapAllocation argv_guard(argv);
	std::vector<std::wstring> result;
	result.reserve(argc);
	for (int i = 0; i < argc; ++i)
	{
		result.emplace_back(argv[i]);
	}
	return result;
}
