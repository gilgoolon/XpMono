#include "BoardFilesystem.hpp"

BoardFilesystem::BoardFilesystem() : m_flash_block_device(1 * 1024 * 1024, 512 * 1024),
                                     m_flash_internal_device(1 * 1024 * 1024 + 512 * 1024, 0),
                                     m_internal_filesystem(std::make_unique<LittlefsFilesystem>()),
                                     m_external_filesystem(std::make_unique<FatFilesystem>()),
                                     m_internal_mount(m_flash_internal_device.mount("/internal", m_internal_filesystem))
{
}
