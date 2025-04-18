#pragma once

#include <filesystem>
#include <vector>

class File final
{
public:
    explicit File(std::filesystem::path path);
    ~File() = default;
    File(const File&) = delete;
    File& operator=(const File&) = delete;
    File(File&&) = delete;
    File& operator=(File&&) = delete;

public:
    void write(const std::vector<uint8_t> &data);
    std::string read_text() const;

private:
    std::filesystem::path m_path;
};
