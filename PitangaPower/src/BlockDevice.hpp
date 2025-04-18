#pragma once

#include "BlockFilesystem.hpp"
#include "MountedBlockFilesystem.hpp"

#include <filesystem/filesystem.h>

class BlockDevice
{
public:
    explicit BlockDevice(const uint32_t start, const size_t size);
    ~BlockDevice();
    BlockDevice(const BlockDevice &) = delete;
    BlockDevice &operator=(const BlockDevice &) = delete;
    BlockDevice(BlockDevice &&) = delete;
    BlockDevice &operator=(BlockDevice &&) = delete;

public:
    void format(const BlockFilesystem::Ptr &filesystem);
    MountedBlockFilesystem mount(std::filesystem::path mount_point, const BlockFilesystem::Ptr &filesystem);

private:
    static blockdevice_t* create_block_device(const uint32_t start, const size_t size);

private:
    blockdevice_t* m_blockdevice;
};
