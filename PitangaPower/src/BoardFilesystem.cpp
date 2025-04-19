#include "BoardFilesystem.hpp"

#include "Config.hpp"

BoardFilesystem::BoardFilesystem() : m_flash_block_device(1 * 1024 * 1024, 512 * 1024),
                                     m_flash_block_device_end_marker(1 * 1024 * 1024 + 512 * 1024, 0),
                                     m_internal_filesystem(std::make_unique<LittlefsFilesystem>()),
                                     m_external_filesystem(std::make_unique<FatFilesystem>())
{
    m_flash_block_device.format(m_internal_filesystem);
    m_internal_mount = m_flash_block_device.mount(Config::INTERNAL_FILESYSTEM_ROOT, m_internal_filesystem);
}
