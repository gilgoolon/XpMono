#include "Filesystem/FileEntry.hpp"

#include "Exception.hpp"
#include "Utils/Sizes.hpp"
#include "Utils/Strings.hpp"

static const WIN32_FIND_DATAW& get_validated_data(const Buffer& find_data)
{
	if (find_data.size() < sizeof(WIN32_FIND_DATAW))
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}
	return *reinterpret_cast<const WIN32_FIND_DATAW*>(find_data.data());
}

FileEntry::FileEntry(const std::filesystem::path& root, const Buffer& find_data):
	FileEntry(root, get_validated_data(find_data))
{
}

FileEntry::FileEntry(const std::filesystem::path& root, const WIN32_FIND_DATAW& data):
	m_path(root / data.cFileName),
	m_type((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0 ? Type::DIRECTORY : Type::FILE),
	m_size(Sizes::make_large_int(data.nFileSizeLow, data.nFileSizeHigh)),
	m_creation(Time::to_datetime(data.ftCreationTime)),
	m_modification(Time::to_datetime(data.ftLastWriteTime)),
	m_access(Time::to_datetime(data.ftLastAccessTime))
{
}

static std::wostream& operator<<(std::wostream& stream, const FileEntry::Type type)
{
	switch (type)
	{
	case FileEntry::Type::FILE:
		stream << L"File";
		break;
	case FileEntry::Type::DIRECTORY:
		stream << L"Directory";
		break;
	}
	return stream;
}

Buffer FileEntry::serialize() const
{
	std::wostringstream out;
	static constexpr auto SEPARATOR = L" | ";
	out << L"Path: " << m_path << SEPARATOR
		<< L"Type: " << m_type << SEPARATOR
		<< L"Size: " << m_size << SEPARATOR
		<< L"Creation Time: " << Strings::to_wstring(m_creation) << SEPARATOR
		<< L"Modification Time: " << Strings::to_wstring(m_modification) << SEPARATOR
		<< L"Access Time: " << Strings::to_wstring(m_access);
	return Strings::to_buffer(out.str());
}
