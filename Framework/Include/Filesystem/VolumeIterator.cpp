#include "VolumeIterator.hpp"

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
			TRACE(L"failed to close volume handle")
		}
	}
	catch (...)
	{
		TRACE(L"failed to close volume handle")
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
	if (result != TRUE)
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
		nullptr,
		size,
		&size
	);
	if (GetLastError() != ERROR_MORE_DATA)
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
