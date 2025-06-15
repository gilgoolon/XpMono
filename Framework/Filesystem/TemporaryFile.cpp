#include "Filesystem/TemporaryFile.hpp"

#include "Trace.hpp"
#include "Filesystem/File.hpp"
#include "Utils/Environment.hpp"

TemporaryFile::TemporaryFile():
	File(generate_temp_path(), Mode::READ_WRITE, Disposition::CREATE)
{
}

TemporaryFile::TemporaryFile(const std::filesystem::path& source):
	File(copy(source, generate_temp_path()), Mode::READ_WRITE, Disposition::OPEN)
{
}

TemporaryFile::~TemporaryFile()
{
	try
	{
		const std::filesystem::path file_path = path();
		m_handle.reset();
		unlink(file_path);
	}
	catch (...)
	{
		TRACE(L"failed to delete temporary file");
	}
}

std::filesystem::path TemporaryFile::generate_temp_path()
{
	static constexpr auto TEMP_PREFIX = L"tmp";
	static constexpr UINT ENFORCE_UNIQUE_FILE = 0;

	static constexpr wchar_t NULL_TERMINATOR = L'\0';
	std::wstring buffer(MAX_PATH, NULL_TERMINATOR);

	const std::filesystem::path temp_folder = Environment::get_temp_folder();

	const UINT result = GetTempFileNameW(
		temp_folder.wstring().c_str(),
		TEMP_PREFIX,
		ENFORCE_UNIQUE_FILE,
		buffer.data()
	);

	if (result == FALSE)
	{
		throw WinApiException(ErrorCode::FAILED_GENERATE_TEMP_PATH);
	}

	static constexpr auto TEMP_SUFFIX = L".TMP";
	return temp_folder / Formatting::format(L"%s%04x%s", TEMP_SUFFIX, static_cast<uint16_t>(result), TEMP_SUFFIX);
}
