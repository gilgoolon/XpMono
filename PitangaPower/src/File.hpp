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
    std::vector<uint8_t> read_all();

private:
    std::filesystem::path _path;
};
