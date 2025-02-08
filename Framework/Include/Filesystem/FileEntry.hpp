#pragma once
#include "Interfaces/ISerializable.hpp"
#include "Utils/Buffer.hpp"
#include "Utils/Time.hpp"

#include <filesystem>

class FileEntry final : public ISerializable
{
	FileEntry(const std::filesystem::path& root, const WIN32_FIND_DATAW& data);

public:
	explicit FileEntry(const std::filesystem::path& root, const Buffer& find_data);
	~FileEntry() override = default;
	FileEntry(const FileEntry&) = delete;
	FileEntry& operator=(const FileEntry&) = delete;
	FileEntry(FileEntry&&) = delete;
	FileEntry& operator=(FileEntry&&) = delete;

	enum class Type : uint32_t
	{
		FILE = 0,
		DIRECTORY
	};

	[[nodiscard]] Buffer serialize() const override;

private:
	std::filesystem::path m_path;
	Type m_type;
	uint64_t m_size;
	Time::Datetime m_creation;
	Time::Datetime m_modification;
	Time::Datetime m_access;

	friend class FileIterator;
};
