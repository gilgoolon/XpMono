#pragma once

#include <filesystem>
#include "BlockFilesystem.hpp"

class MountedBlockFilesystem
{
public:
    explicit MountedBlockFilesystem(std::filesystem::path mount_point);
    ~MountedBlockFilesystem();
    MountedBlockFilesystem(const MountedBlockFilesystem&) = delete;
    MountedBlockFilesystem &operator=(const MountedBlockFilesystem&) = delete;
    MountedBlockFilesystem(MountedBlockFilesystem&&) = default;
    MountedBlockFilesystem &operator=(MountedBlockFilesystem&&) = default;

private:
    std::filesystem::path m_root;
};
