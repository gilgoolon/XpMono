#pragma once
#include "Interfaces/IIOStream.hpp"
#include "Processes/ScopedHandle.hpp"
#include "Utils/Buffer.hpp"

#include <filesystem>
#include <Windows.h>

class File final : public IIOStream
{
public:
	enum class Mode : uint32_t
	{
		READ = FILE_GENERIC_READ,
		WRITE = FILE_GENERIC_WRITE,
		READ_WRITE = FILE_GENERIC_READ | FILE_GENERIC_WRITE,
	};

	enum class Disposition : uint32_t
	{
		OPEN = OPEN_EXISTING,
		CREATE = CREATE_NEW,
		OVERRIDE = CREATE_ALWAYS,
	};

	explicit File(const std::filesystem::path& path, Mode mode, Disposition disposition);
	~File() override = default;
	File(const File&) = delete;
	File& operator=(const File&) = delete;
	File(File&&) = default;
	File& operator=(File&&) = default;

	[[nodiscard]] Buffer read() const;
	[[nodiscard]] Buffer read(uint32_t size) const override;
	[[nodiscard]] Buffer read(uint64_t offset, uint32_t size) const;

	void write(const Buffer& data) const override;
	void write(const Buffer& data, uint64_t offset) const;

	void seek(uint64_t offset) const;

	void set_metadata_of(const File& file);

private:
	ScopedHandle m_handle;

	[[nodiscard]] static HANDLE create_file(const std::filesystem::path& path, Mode mode, Disposition disposition);
};
