#pragma once

#include <filesystem>
#include <vector>

class File final
{
public:
    explicit File(std::filesystem::path path);
    ~File();
    File(const File&) = delete;
    File& operator=(const File&) = delete;
    File(File&&) = delete;
    File& operator=(File&&) = delete;

private:
    int create_file(std::filesystem::path path);

public:
    void write(const std::vector<uint8_t> &data);
    std::vector<uint8_t> read_all() const;

private:
    int m_fd;
};
