#pragma once
#include "RemoteMemory.hpp"
#include "Utils/Buffer.hpp"

#include "ScopedHandle.hpp"
#include "Synchronization/IWaitable.hpp"
#include "Utils/Time.hpp"

#include <memory>
#include <string>
#include <vector>
#include <winternl.h>

class Process final : public std::enable_shared_from_this<Process>, public IWaitable
{
public:
	using Ptr = std::unique_ptr<Process>;

	explicit Process(uint32_t pid);
	explicit Process(const std::wstring& command_line);
	~Process() override = default;
	Process(const Process&) = delete;
	Process& operator=(const Process&) = delete;
	Process(Process&&) = delete;
	Process& operator=(Process&&) = delete;

private:
	[[nodiscard]] HANDLE handle() const override;

	[[nodiscard]] static HANDLE open_process(uint32_t pid);

	[[nodiscard]] static HANDLE create_process(const std::wstring& command_line);

	[[nodiscard]] PROCESS_BASIC_INFORMATION query_basic_information() const;

public:
	[[nodiscard]] uint32_t get_pid() const;

	[[nodiscard]] std::wstring get_path() const;

	[[nodiscard]] bool is_running() const;

	[[nodiscard]] Time::Datetime get_creation_time() const;

	[[nodiscard]] std::string get_command_line() const;

	void terminate();

	[[nodiscard]] Buffer read_memory(const void* address, size_t size) const;

	[[nodiscard]] RemoteMemory::Ptr allocate_memory(uint32_t size);

	[[nodiscard]] std::vector<HMODULE> get_modules() const;

	[[nodiscard]] HMODULE get_main_module() const;

private:
	ScopedHandle m_handle;

	friend RemoteMemory;
};
