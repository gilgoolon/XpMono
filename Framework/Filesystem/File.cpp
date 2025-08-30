#include "Filesystem/File.hpp"

#include "Exception.hpp"

File::File(const std::filesystem::path& path,
           const Mode mode,
           const Disposition disposition,
           const Share share):
	m_handle(create_file(path, mode, disposition, share))
{
}

Buffer File::read() const
{
	static constexpr LPOVERLAPPED UNUSED_OVERLAPPED = nullptr;
	static constexpr uint32_t BUFF_SIZE = 4096;
	Buffer buff;
	DWORD bytes_read = 0;
	do
	{
		buff.resize(buff.size() + BUFF_SIZE);
		const BOOL result = ReadFile(
			m_handle.get(),
			buff.data() + buff.size() - BUFF_SIZE,
			BUFF_SIZE,
			&bytes_read,
			UNUSED_OVERLAPPED
		);
		if (result == FALSE)
		{
			throw WinApiException(ErrorCode::FAILED_FILE_READ);
		}
	}
	while (bytes_read >= BUFF_SIZE);
	buff.resize(buff.size() - BUFF_SIZE + bytes_read);
	return buff;
}

Buffer File::read(const uint32_t size) const
{
	static constexpr LPOVERLAPPED UNUSED_OVERLAPPED = nullptr;
	Buffer buff(size);
	DWORD bytes_read = 0;
	const BOOL result = ReadFile(
		m_handle.get(),
		buff.data(),
		size,
		&bytes_read,
		UNUSED_OVERLAPPED
	);
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_READ);
	}
	buff.resize(bytes_read);
	return buff;
}

Buffer File::read(const uint64_t offset, const uint32_t size) const
{
	seek(offset);
	return read(size);
}

void File::write(const Buffer& data) const
{
	static constexpr LPOVERLAPPED UNUSED_OVERLAPPED = nullptr;
	DWORD bytes_written = 0;
	const BOOL result = WriteFile(
		m_handle.get(),
		data.data(),
		static_cast<uint32_t>(data.size()),
		&bytes_written,
		UNUSED_OVERLAPPED
	);
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_WRITE);
	}
}

void File::write(const Buffer& data, const uint64_t offset) const
{
	seek(offset);
	write(data);
}

void File::seek(const uint64_t offset) const
{
	const LONG offset_low = static_cast<LONG>(offset);
	LONG offset_high = static_cast<LONG>(offset >> 32);
	static constexpr WORD COUNT_FROM_BEGINNING = FILE_BEGIN;
	const DWORD result = SetFilePointer(m_handle.get(), offset_low, &offset_high, COUNT_FROM_BEGINNING);
	if (result == INVALID_SET_FILE_POINTER)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_SEEK);
	}
}

void File::set_metadata_of(const File& file)
{
	FILETIME creation{};
	FILETIME access{};
	FILETIME modification{};

	BOOL result =
		GetFileTime(file.m_handle.get(), &creation, &access, &modification);
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_GET_TIME);
	}
	result = SetFileTime(m_handle.get(), &creation, &access, &modification);
	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_SET_TIME);
	}
}

std::filesystem::path File::path() const
{
	static constexpr wchar_t NULL_TERMINATOR = L'\0';
	std::wstring path(MAX_PATH, NULL_TERMINATOR);
	const DWORD result = GetFinalPathNameByHandleW(
		m_handle.get(),
		path.data(),
		static_cast<uint32_t>(path.size()),
		FILE_NAME_NORMALIZED
	);

	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_GET_PATH);
	}

	path.resize(result);
	return path;
}

void File::unlink(const std::filesystem::path& path)
{
	if (DeleteFileW(path.wstring().c_str()) == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_UNLINK);
	}
}

std::filesystem::path File::copy(const std::filesystem::path& source, const std::filesystem::path& destination)
{
	static constexpr BOOL EXISTS_OK = FALSE;
	if (CopyFileW(source.wstring().c_str(), destination.wstring().c_str(), EXISTS_OK) == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_COPY);
	}
	return destination;
}

HANDLE File::create_file(const std::filesystem::path& path, Mode mode, Disposition disposition, Share share)
{
	static constexpr LPSECURITY_ATTRIBUTES DEFAULT_SECURITY = nullptr;
	static constexpr DWORD REGULAR_FILE = 0;
	static constexpr HANDLE EMPTY_FILE = nullptr;
	const DWORD creation_disposition = static_cast<DWORD>(disposition);
	const DWORD access = static_cast<DWORD>(mode);
	const DWORD file_share = static_cast<DWORD>(share);
	const HANDLE result = CreateFileW(
		path.wstring().c_str(),
		access,
		file_share,
		DEFAULT_SECURITY,
		creation_disposition,
		REGULAR_FILE,
		EMPTY_FILE
	);
	if (result == INVALID_HANDLE_VALUE)
	{
		throw WinApiException(ErrorCode::FAILED_FILE_CREATE);
	}
	return result;
}
