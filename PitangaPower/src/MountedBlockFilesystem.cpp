#include "MountedBlockFilesystem.hpp"

#include <filesystem/vfs.h>

MountedBlockFilesystem::MountedBlockFilesystem(std::filesystem::path mount_point):
    m_root(std::move(mount_point))
{
}

MountedBlockFilesystem::~MountedBlockFilesystem()
{
    try
    {
        fs_unmount(m_root.string().c_str());
    }
    catch(...)
    {
    }
}
