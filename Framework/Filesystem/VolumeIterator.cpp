#include "Filesystem/VolumeIterator.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Utils/Strings.hpp"

static constexpr uint32_t MAX_GUID_LENGTH = MAX_PATH;

VolumeIterator::VolumeIterator():
	m_find_handle(nullptr),
	m_finished_find(false),
	m_entries{}
{
	static constexpr wchar_t NULL_TERM = L'\0';
	std::wstring buffer(MAX_GUID_LENGTH, NULL_TERM);
	m_find_handle = FindFirstVolumeW(buffer.data(), MAX_GUID_LENGTH);
	if (m_find_handle == INVALID_HANDLE_VALUE)
	{
		throw WinApiException(ErrorCode::FAILED_VOLUME_FIND_FIRST);
	}
	add_guid_paths(buffer);
}

VolumeIterator::~VolumeIterator()
{
	try
	{
		if (FindClose(m_find_handle) == FALSE)
		{
			TRACE(L"failed to close volume handle");
		}
	}
	catch (...)
	{
		TRACE(L"failed to close volume handle");
	}
}

void VolumeIterator::add_guid_paths(const std::wstring& guid)
{
	DWORD size = 0;
	std::wstring buffer;
	BOOL result = GetVolumePathNamesForVolumeNameW(
		guid.c_str(),
		buffer.data(),
		size,
		&size
	);
	if (result != FALSE)
	{
		return;
	}
	if (GetLastError() != ERROR_MORE_DATA)
	{
		throw WinApiException(ErrorCode::FAILED_VOLUME_GET_PATHS);
	}
	buffer.resize(size);
	result = GetVolumePathNamesForVolumeNameW(
		guid.c_str(),
		buffer.data(),
		size,
		&size
	);
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_VOLUME_GET_PATHS);
	}
	buffer.resize(size);
	for (const std::wstring& path : Strings::parse_raw_strings(buffer))
	{
		m_entries.emplace(path);
	}
}

void VolumeIterator::try_retrieve_next()
{
	if (!m_entries.empty() || m_finished_find)
	{
		return;
	}
	static constexpr wchar_t NULL_TERM = L'\0';
	std::wstring buffer(MAX_GUID_LENGTH, NULL_TERM);
	if (FindNextVolumeW(m_find_handle, buffer.data(), MAX_GUID_LENGTH) == FALSE)
	{
		if (GetLastError() == ERROR_NO_MORE_FILES)
		{
			m_finished_find = true;
			return;
		}
		throw WinApiException(ErrorCode::FAILED_VOLUME_FIND_NEXT);
	}
	add_guid_paths(buffer);
}

bool VolumeIterator::has_next() const
{
	return !m_entries.empty();
}

std::filesystem::path VolumeIterator::next()
{
	if (!has_next())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}
	const std::filesystem::path result = m_entries.front();
	m_entries.pop();
	try_retrieve_next();
	return result;
}

std::filesystem::path VolumeIterator::get_system_volume()
{
	static constexpr wchar_t NULL_TERM = L'\0';
	std::string result(MAX_GUID_LENGTH, NULL_TERM);

	const UINT chars_written = GetSystemDirectoryA(result.data(), result.size());
	if (chars_written == 0)
	{
		throw WinApiException(ErrorCode::FAILED_VOLUME_GET_SYSTEM);
	}
	result.resize(chars_written);

	static constexpr wchar_t DRIVE_LETTERS_SEPARATOR = L':';
	const std::vector<std::string> tokens = Strings::split(result, DRIVE_LETTERS_SEPARATOR);
	if (tokens.size() < 2)
	{
		throw WinApiException(ErrorCode::FAILED_VOLUME_GET_SYSTEM);
	}

	return tokens.front();
}

uint32_t VolumeIterator::get_volume_serial(const std::filesystem::path& volume_path)
{
	static constexpr LPWSTR IGNORED_NAME = nullptr;
	static constexpr DWORD IGNORED_NAME_SIZE = 0;
	static constexpr LPDWORD IGNORED_MAX_COMPONENT_LENGTH = nullptr;
	static constexpr LPDWORD DEFAULT_FLAGS = nullptr;
	static constexpr LPWSTR IGNORED_FILESYSTEM_NAME = nullptr;
	static constexpr DWORD IGNORED_FILESYSTEM_NAME_SIZE = 0;

	DWORD serial_number = 0;
	const BOOL result = GetVolumeInformationW(
		volume_path.wstring().data(),
		IGNORED_NAME,
		IGNORED_NAME_SIZE,
		&serial_number,
		IGNORED_MAX_COMPONENT_LENGTH,
		DEFAULT_FLAGS,
		IGNORED_FILESYSTEM_NAME,
		IGNORED_FILESYSTEM_NAME_SIZE
	);

	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_VOLUME_GET_SERIAL);
	}

	return serial_number;
}
