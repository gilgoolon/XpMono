#include "Processes/Process.hpp"

#include "Exception.hpp"
#include "Processes/DynamicLibrary.hpp"
#include "Utils/Strings.hpp"

#include <Psapi.h>
#include <Windows.h>
#include <winternl.h>

#pragma comment(lib, "ntdll.lib")

Process::Process(const uint32_t pid, const DWORD access):
	m_handle(open_process(pid, access))
{
}

Process::Process(const std::wstring& command_line):
	m_handle(create_process(command_line))
{
}

HANDLE Process::handle() const
{
	return m_handle.get();
}

HANDLE Process::open_process(const uint32_t pid, const DWORD access)
{
	static constexpr BOOL DONT_INHERIT_HANDLE = FALSE;

	const HANDLE result = OpenProcess(access, DONT_INHERIT_HANDLE, pid);
	if (result == nullptr)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_OPEN);
	}
	return result;
}

HANDLE Process::create_process(const std::wstring& command_line)
{
	static constexpr LPCWSTR NAME_FROM_COMMAND_LINE = nullptr;
	static constexpr LPSECURITY_ATTRIBUTES DEFAULT_SECURITY = nullptr;
	static constexpr BOOL DONT_INHERIT_HANDLE = FALSE;
	static constexpr DWORD RUN_ON_CREATION = 0;
	static constexpr LPVOID THIS_PROCESS_ENVIRONMENT = nullptr;
	static constexpr LPCWSTR THIS_PROCESS_CWD = nullptr;

	std::wstring command_line_copy = command_line;
	STARTUPINFOW startup_info{};
	PROCESS_INFORMATION process_information{};
	const BOOL result = CreateProcessW(
		NAME_FROM_COMMAND_LINE,
		command_line_copy.data(),
		DEFAULT_SECURITY,
		DEFAULT_SECURITY,
		DONT_INHERIT_HANDLE,
		RUN_ON_CREATION,
		THIS_PROCESS_ENVIRONMENT,
		THIS_PROCESS_CWD,
		&startup_info,
		&process_information
	);
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_CREATE);
	}
	ScopedHandle thread_handle_guard(process_information.hThread);
	return process_information.hProcess;
}

using NtQueryInformationProcessFunc = NTSTATUS(WINAPI*)(
	HANDLE process_handle,
	PROCESSINFOCLASS process_information_class,
	PVOID process_information,
	ULONG process_information_length,
	PULONG return_length);

PROCESS_BASIC_INFORMATION Process::query_basic_information() const
{
	const DynamicLibrary loaded_dll("ntdll.dll");
	PROCESS_BASIC_INFORMATION pbi;
	ULONG return_length = 0;
	const NTSTATUS result = loaded_dll.call<NtQueryInformationProcessFunc>(
		"NtQueryInformationProcess",
		m_handle.get(),
		ProcessBasicInformation,
		&pbi,
		static_cast<ULONG>(sizeof(pbi)),
		&return_length
	);
	if (!NT_SUCCESS(result))
	{
		throw WinApiNtException(ErrorCode::FAILED_PROCESS_QUERY_INFORMATION, result);
	}
	return pbi;
}

uint32_t Process::get_pid() const
{
	const uint32_t result = GetProcessId(m_handle.get());
	if (result == 0)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_GET_PID);
	}
	return result;
}

std::wstring Process::get_path() const
{
	static constexpr HMODULE MAIN_MODULE_EXE = nullptr;
	static constexpr char NULL_TERMINATOR = '\0';

	std::wstring path(MAX_PATH, NULL_TERMINATOR);
	const DWORD result = GetModuleFileNameExW(
		m_handle.get(),
		MAIN_MODULE_EXE,
		path.data(),
		static_cast<uint32_t>(path.size())
	);
	if (result == 0)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_GET_PATH);
	}
	path.resize(result);
	return path;
}

bool Process::is_running() const
{
	const WaitStatus result = wait(Time::INSTANT);
	switch (result)
	{
	case WaitStatus::FINISHED:
		[[fallthrough]];
	case WaitStatus::OBJECT_CLOSED:
		return true;

	case WaitStatus::TIMEOUT:
		return false;

	case WaitStatus::FAILED:
		[[fallthrough]];
	default:
		throw WinApiException(ErrorCode::FAILED_WAIT);
	}
}

Time::Datetime Process::get_creation_time() const
{
	FILETIME creation_time{};
	FILETIME exit_time{};
	FILETIME kernel_time{};
	FILETIME user_time{};
	if (GetProcessTimes(
		m_handle.get(),
		&creation_time,
		&exit_time,
		&kernel_time,
		&user_time
	) == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_GET_TIMES);
	}
	return Time::to_datetime(creation_time);
}

std::string Process::get_command_line() const
{
	const PROCESS_BASIC_INFORMATION basic_information = query_basic_information();

	const auto raw_peb = read_memory(basic_information.PebBaseAddress, sizeof(PEB));
	if (!raw_peb)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_READ_MEMORY);
	}
	const auto* const peb = reinterpret_cast<const PEB*>(raw_peb.value().data());

	const auto raw_process_parameters = read_memory(peb->ProcessParameters, sizeof(RTL_USER_PROCESS_PARAMETERS));
	if (!raw_process_parameters)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_READ_MEMORY);
	}
	const auto* const process_parameters = reinterpret_cast<const RTL_USER_PROCESS_PARAMETERS*>(
		raw_process_parameters.value().data());

	const auto raw_command_line = read_memory(
		process_parameters->CommandLine.Buffer,
		process_parameters->CommandLine.Length
	);
	if (!raw_command_line)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_READ_MEMORY);
	}
	return Strings::to_string(Strings::to_wstring(raw_command_line.value()));
}

void Process::terminate() const
{
	static constexpr DWORD EXIT_CODE = EXIT_FAILURE;

	if (TerminateProcess(m_handle.get(), EXIT_CODE) == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_TERMINATE);
	}
}

std::optional<Buffer> Process::read_memory(const void* const address, const size_t size) const
{
	Buffer buffer(size);
	const BOOL result = ReadProcessMemory(m_handle.get(), address, buffer.data(), size, nullptr);
	if (result == FALSE)
	{
		return std::nullopt;
	}
	return buffer;
}

RemoteMemory::Ptr Process::allocate_memory(uint32_t size)
{
	struct PublicRemoteMemory : RemoteMemory
	{
		PublicRemoteMemory(std::weak_ptr<Process> process, const uint32_t size):
			RemoteMemory(std::move(process), size)
		{
		}
	};
	return std::make_unique<PublicRemoteMemory>(weak_from_this(), size);
}

std::vector<HMODULE> Process::get_modules() const
{
	static constexpr HMODULE* NO_BUFFER = nullptr;
	static constexpr DWORD EMPTY_BUFFER_SIZE = 0;

	DWORD needed_buffer_size = 0;
	BOOL result = EnumProcessModules(
		m_handle.get(),
		NO_BUFFER,
		EMPTY_BUFFER_SIZE,
		&needed_buffer_size
	);
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_GET_MODULES);
	}

	std::vector<HMODULE> buffer(needed_buffer_size);
	needed_buffer_size = 0;
	result = EnumProcessModules(
		m_handle.get(),
		buffer.data(),
		static_cast<uint32_t>(buffer.size() * sizeof(HMODULE)),
		&needed_buffer_size
	);

	if (result != FALSE)
	{
		return buffer;
	}
	throw WinApiException(ErrorCode::FAILED_PROCESS_GET_MODULES);
}

HMODULE Process::get_module(const std::wstring& name) const
{
	const std::vector<HMODULE> modules = get_modules();
	for (const HMODULE& loaded_module : modules)
	{
		const std::wstring module_name = get_module_name(loaded_module);
		if (name == module_name)
		{
			return loaded_module;
		}
	}
	throw WinApiException(ErrorCode::FAILED_PROCESS_GET_MODULE);
}

std::wstring Process::get_module_name(const HMODULE loaded_module) const
{
	static constexpr wchar_t NULL_TERMINATOR = L'\0';

	std::wstring name(MAX_PATH, NULL_TERMINATOR);
	const DWORD result = GetModuleBaseNameW(
		m_handle.get(),
		loaded_module,
		name.data(),
		static_cast<uint32_t>(name.size())
	);
	static constexpr DWORD FAILED = 0;
	if (result == FAILED)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_GET_MODULE_NAME);
	}
	name.resize(result);
	return name;
}

std::wstring Process::get_module_path(const HMODULE loaded_module) const
{
	static constexpr wchar_t NULL_TERMINATOR = L'\0';

	std::wstring path(MAX_PATH, NULL_TERMINATOR);
	const DWORD result = GetModuleFileNameExW(
		m_handle.get(),
		loaded_module,
		path.data(),
		static_cast<uint32_t>(path.size())
	);
	static constexpr DWORD FAILED = 0;
	if (result == FAILED)
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_GET_MODULE_FILENAME);
	}
	path.resize(result);
	return path;
}

HMODULE Process::get_main_module() const
{
	const std::wstring process_filename = get_path();
	for (const HMODULE& loaded_module : get_modules())
	{
		if (get_module_path(loaded_module) == process_filename)
		{
			return loaded_module;
		}
	}
	throw WinApiException(ErrorCode::FAILED_PROCESS_GET_MAIN_MODULE);
}

bool Process::is_address_in_rdata_section_of_module(const HMODULE module, const uintptr_t address) const
{
	auto raw_dos_header = read_memory(module, sizeof(IMAGE_DOS_HEADER));
	if (!raw_dos_header)
	{
		return false;
	}
	auto dos_header = *reinterpret_cast<IMAGE_DOS_HEADER*>(raw_dos_header.value().data());

	auto raw_nt_headers = read_memory(
		reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(module) + dos_header.e_lfanew),
		sizeof(IMAGE_NT_HEADERS)
	);
	if (!raw_nt_headers)
	{
		return false;
	}
	IMAGE_NT_HEADERS nt_headers = *reinterpret_cast<IMAGE_NT_HEADERS*>(raw_nt_headers.value().data());

	uint16_t num_sections = nt_headers.FileHeader.NumberOfSections;
	auto raw_sections_headers = read_memory(
		reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(module) + dos_header.e_lfanew + sizeof(
			IMAGE_NT_HEADERS)),
		num_sections * sizeof(IMAGE_SECTION_HEADER)
	);
	if (!raw_sections_headers)
	{
		return false;
	}
	auto sections_headers = reinterpret_cast<PIMAGE_SECTION_HEADER>(raw_sections_headers.value().
		data());

	for (uint16_t i = 0; i < num_sections; i++)
	{
		const IMAGE_SECTION_HEADER& section_header = sections_headers[i];
		auto raw_name = reinterpret_cast<const char*>(section_header.Name);
		const std::string section_name(raw_name, strnlen(raw_name, IMAGE_SIZEOF_SHORT_NAME));

		if (section_name != ".rdata")
		{
			continue;
		}

		const uintptr_t section_start = reinterpret_cast<uintptr_t>(module) + section_header.VirtualAddress;
		const uintptr_t section_end = section_start + section_header.Misc.VirtualSize;

		if (address >= section_start && address < section_end)
		{
			return true;
		}
		break;
	}

	return false;
}

MEMORY_BASIC_INFORMATION Process::get_region_info(const void* address) const
{
	MEMORY_BASIC_INFORMATION region_info{};
	const SIZE_T result = VirtualQueryEx(m_handle.get(), address, &region_info, sizeof(region_info));
	if (result != sizeof(region_info))
	{
		throw WinApiException(ErrorCode::FAILED_PROCESS_GET_REGION_INFO);
	}
	return region_info;
}
