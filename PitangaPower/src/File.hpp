#pragma once

#include <filesystem>

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
    std::string read_text();

private:
    std::filesystem::path _path;
};
