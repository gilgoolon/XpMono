#include "BlockDevice.hpp"

#include "blockdevice/flash.h"
#include <filesystem/vfs.h>

BlockDevice::BlockDevice(const uint32_t start, const size_t size) : m_blockdevice(blockdevice_flash_create(start, size))
{
}

BlockDevice::~BlockDevice()
{
    try
    {
        blockdevice_flash_free(m_blockdevice);
    }
    catch(...)
    {
    }
    
}

void BlockDevice::format(const BlockFilesystem::Ptr &filesystem)
{
    const int result = fs_format(filesystem->handle(), m_blockdevice);
    static constexpr int FORMAT_FAILED = -1;
    if (result == FORMAT_FAILED)
    {
        throw std::runtime_error("failed to format filesystem on block device");
    }
}

MountedBlockFilesystem::Ptr BlockDevice::mount(std::filesystem::path mount_point, const BlockFilesystem::Ptr &filesystem)
{
    const int result = fs_mount(mount_point.string().c_str(), filesystem->handle(), m_blockdevice);
    static constexpr int MOUNT_FAILED = -1;
    if (result == MOUNT_FAILED)
    {
        throw std::runtime_error("failed to mount filesystem");
    }
    return std::make_shared<MountedBlockFilesystem>(std::move(mount_point));
}

blockdevice_t *BlockDevice::create_block_device(const uint32_t start, const size_t size)
{
    blockdevice_t* const result = blockdevice_flash_create(start, size);
    if (result == nullptr)
    {
        throw std::runtime_error("failed to create block device");
    }
    return result;
}
