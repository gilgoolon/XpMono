#pragma once

#include "BlockDevice.hpp"
#include "FatFilesystem.hpp"
#include "LittlefsFilesystem.hpp"

class BoardFilesystem final
{
public:
    explicit BoardFilesystem();
    ~BoardFilesystem() = default;
    BoardFilesystem(const BoardFilesystem&) = delete;
    BoardFilesystem& operator=(const BoardFilesystem &) = delete;
    BoardFilesystem(BoardFilesystem&&) = delete;
    BoardFilesystem& operator=(BoardFilesystem&&) = delete;

private:
    BlockDevice m_flash_block_device;
    BlockDevice m_flash_internal_device;
    BlockFilesystem::Ptr m_internal_filesystem;
    BlockFilesystem::Ptr m_external_filesystem;
    MountedBlockFilesystem m_internal_mount;
};
