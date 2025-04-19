#pragma once

#include "BlockDevice.hpp"

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
    BlockDevice m_flash_block_device_end_marker;
    BlockFilesystem::Ptr m_internal_filesystem;
    BlockFilesystem::Ptr m_external_filesystem;
    std::shared_ptr<MountedBlockFilesystem> m_internal_mount;
};
