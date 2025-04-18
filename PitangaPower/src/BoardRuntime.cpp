#include "BoardRuntime.hpp"

#include <bsp/board.h>
#include <tusb.h>
#include <filesystem/vfs.h>
#include <stdexcept>

BoardRuntime::BoardRuntime()
{
    board_init();
    tusb_init();
    if (!fs_init())
    {
        throw std::runtime_error("failed to init filesystem");
    }
}