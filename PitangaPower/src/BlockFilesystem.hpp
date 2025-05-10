#pragma once

#include <filesystem/filesystem.h>
#include <memory>

class BlockFilesystem
{
public:
    using Ptr = std::unique_ptr<BlockFilesystem>;

public:
    explicit BlockFilesystem() = default;
    virtual ~BlockFilesystem() = default;
    BlockFilesystem(const BlockFilesystem&) = delete;
    BlockFilesystem& operator=(const BlockFilesystem&) = delete;
    BlockFilesystem(BlockFilesystem&&) = delete;
    BlockFilesystem& operator=(BlockFilesystem&&) = delete;

protected:
    virtual filesystem_t* handle() = 0;

public:
    friend class BlockDevice;
};
