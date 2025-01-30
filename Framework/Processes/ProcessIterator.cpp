#include "Processes/ProcessIterator.hpp"

#include "Exception.hpp"

#include <tlhelp32.h>

ProcessIterator::ProcessIterator():
	m_snapshot_handle(create_processes_snapshot()),
	m_next_result(nullptr)
{
	retrieve_first();
}

std::vector<UnopenedProcess::Ptr> ProcessIterator::get_all()
{
	ProcessIterator enumerator;
	std::vector<UnopenedProcess::Ptr> result;
	while (enumerator.has_next())
	{
		result.push_back(enumerator.next());
	}
	return result;
}

HANDLE ProcessIterator::create_processes_snapshot()
{
	static constexpr DWORD SNAPSHOT_ONLY_PROCESSES = TH32CS_SNAPPROCESS;
	static constexpr DWORD PID_IGNORED = 0;
	const HANDLE result = CreateToolhelp32Snapshot(SNAPSHOT_ONLY_PROCESSES, PID_IGNORED);
	if (result == INVALID_HANDLE_VALUE)
	{
		throw WinApiException(ErrorCode::FAILED_ENUMERATE_PROCESSES);
	}
	return result;
}

void ProcessIterator::retrieve_first() const
{
	PROCESSENTRY32W result{};
	result.dwSize = sizeof(result);
	if (Process32FirstW(m_snapshot_handle.get(), &result) == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_ENUMERATE_PROCESSES);
	}
	m_next_result = std::make_unique<UnopenedProcess>(result.th32ProcessID);
}

void ProcessIterator::retrieve_next() const
{
	PROCESSENTRY32W result{};
	result.dwSize = sizeof(result);
	if (Process32NextW(m_snapshot_handle.get(), &result) == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_ENUMERATE_PROCESSES);
	}
	m_next_result = std::make_unique<UnopenedProcess>(result.th32ProcessID);
}

UnopenedProcess::Ptr ProcessIterator::next()
{
	if (!has_next())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}
	UnopenedProcess::Ptr result = std::move(m_next_result);
	m_next_result = nullptr;
	return std::move(result);
}

bool ProcessIterator::has_next() const
{
	if (m_next_result != nullptr)
	{
		return true;
	}
	try
	{
		retrieve_next();
	}
	catch (const WinApiException& ex)
	{
		if (ex == ErrorCode::FAILED_ENUMERATE_PROCESSES && ex.error() == ERROR_NO_MORE_FILES)
		{
			return false;
		}
		throw;
	}
	return true;
}
