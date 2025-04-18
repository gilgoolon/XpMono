#pragma once

#include "BlockFilesystem.hpp"

class LittlefsFilesystem final : public BlockFilesystem
{
public:
    explicit LittlefsFilesystem(uint32_t block_cycles = 500, uint32_t lookahead_size = 16);
    ~LittlefsFilesystem();

protected:
    filesystem_t* handle() override;

private:
    filesystem_t *create_littlefs_filesystem(uint32_t block_cycles, uint32_t lookahead_size);

private:
    filesystem_t* m_handle;
};
