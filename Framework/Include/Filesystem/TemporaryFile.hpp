#pragma once
#include "File.hpp"

#include <filesystem>

class TemporaryFile final : public File
{
public:
	explicit TemporaryFile();
	explicit TemporaryFile(const std::filesystem::path& source);
	~TemporaryFile() override;
	TemporaryFile(const TemporaryFile&) = delete;
	TemporaryFile& operator=(const TemporaryFile&) = delete;
	TemporaryFile(TemporaryFile&&) = delete;
	TemporaryFile& operator=(TemporaryFile&&) = delete;

private:
	[[nodiscard]] static std::filesystem::path generate_temp_path();
};
