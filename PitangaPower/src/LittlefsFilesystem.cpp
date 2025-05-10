#include "LittlefsFilesystem.hpp"

#include <filesystem/littlefs.h>

LittlefsFilesystem::LittlefsFilesystem(uint32_t block_cycles, uint32_t lookahead_size):
    m_handle(create_littlefs_filesystem(block_cycles, lookahead_size))
{
}

LittlefsFilesystem::~LittlefsFilesystem()
{
    try
    {
        filesystem_littlefs_free(m_handle);
    }
    catch(...)
    {
    }
    
}

filesystem_t *LittlefsFilesystem::handle()
{
    return m_handle;
}

filesystem_t *LittlefsFilesystem::create_littlefs_filesystem(uint32_t block_cycles, uint32_t lookahead_size)
{
    filesystem_t *const result = filesystem_littlefs_create(block_cycles, lookahead_size);
    if (result == nullptr)
    {
        throw std::runtime_error("failed to create littlefs filesystem");
    }
    return result;
}
