#include "Processes/RemoteMemory.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Processes/Process.hpp"

RemoteMemory::RemoteMemory(std::weak_ptr<Process> process, const uint32_t size):
	m_process(std::move(process)),
	m_base_address(allocate_remote_memory(*m_process.lock(), size))
{
}

RemoteMemory::~RemoteMemory()
{
	try
	{
		static constexpr DWORD SIZE_WHEN_ALL = 0;
		if (VirtualFreeEx(m_process.lock()->m_handle.get(), m_base_address, SIZE_WHEN_ALL, MEM_RELEASE) == FALSE)
		{
			TRACE(L"failed freeing remote memory");
		}
	}
	catch (...)
	{
		TRACE(L"failed freeing remote memory");
	}
}

void RemoteMemory::write(const std::vector<uint8_t>& data)
{
	DWORD bytes_written = 0;
	const BOOL result = WriteProcessMemory(
		m_process.lock()->m_handle.get(),
		m_base_address,
		data.data(),
		data.size(),
		&bytes_written
	);
	if (result == FALSE || bytes_written != data.size())
	{
		throw WinApiException(ErrorCode::FAILED_REMOTE_MEMORY_WRITE);
	}
}

void* RemoteMemory::allocate_remote_memory(Process& process, const uint32_t size)
{
	static constexpr LPVOID UNSPECIFIED_ADDRESS = nullptr;
	static constexpr DWORD READ_WRITE_EXECUTE = PAGE_EXECUTE_READWRITE;
	const LPVOID result = VirtualAllocEx(
		process.m_handle.get(),
		UNSPECIFIED_ADDRESS,
		size,
		MEM_RESERVE | MEM_COMMIT,
		READ_WRITE_EXECUTE
	);
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_REMOTE_MEMORY_ALLOCATE);
	}
	return result;
}

void* RemoteMemory::get() const
{
	return m_base_address;
}
