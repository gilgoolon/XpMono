#include "FatFilesystem.hpp"

#include <filesystem/fat.h>
#include <stdexcept>

FatFilesystem::FatFilesystem():
    m_handle(create_fat_filesystem())
{
}

FatFilesystem::~FatFilesystem()
{
    try
    {
        filesystem_fat_free(m_handle);
    }
    catch(...)
    {
    }
    
}

filesystem_t *FatFilesystem::handle()
{
    return m_handle;
}

filesystem_t *FatFilesystem::create_fat_filesystem()
{
    filesystem_t* const result = filesystem_fat_create();
    if (result == nullptr)
    {
        throw std::runtime_error("failed to create FAT filesystem");
    }
    return result;
}
