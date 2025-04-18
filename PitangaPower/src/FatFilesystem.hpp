#pragma once

#include "BlockFilesystem.hpp"

class FatFilesystem final : public BlockFilesystem
{
public:
    explicit FatFilesystem();
    ~FatFilesystem() override;
    FatFilesystem(const FatFilesystem &) = delete;
    FatFilesystem &operator=(const FatFilesystem &) = delete;
    FatFilesystem(FatFilesystem &&) = delete;
    FatFilesystem &operator=(FatFilesystem &&) = delete;

protected:
    filesystem_t* handle() override;

private:
    filesystem_t* create_fat_filesystem();

private:
    filesystem_t* m_handle;
};
