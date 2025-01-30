#pragma once
#include "Processes/ScopedHandle.hpp"
#include "Utils/Buffer.hpp"

#include <filesystem>
#include <Windows.h>

class File final
{
public:
	enum class Mode : uint32_t
	{
		READ = FILE_GENERIC_READ,
		WRITE = FILE_GENERIC_WRITE,
	};

	explicit File(const std::filesystem::path& path, Mode mode);
	~File() = default;
	File(const File&) = delete;
	File& operator=(const File&) = delete;
	File(File&&) = delete;
	File& operator=(File&&) = delete;

	[[nodiscard]] Buffer read() const;
	[[nodiscard]] Buffer read(uint32_t size) const;
	[[nodiscard]] Buffer read(uint64_t offset, uint32_t size) const;

	void write(const Buffer& data);
	void write(const Buffer& data, uint64_t offset);

	void seek(uint64_t offset) const;

private:
	ScopedHandle m_handle;

	[[nodiscard]] static HANDLE create_file(const std::filesystem::path&, Mode mode);
};
