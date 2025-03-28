#include "BoardRuntime.hpp"

#include "bsp/board.h"
#include "tusb.h"

BoardRuntime::BoardRuntime()
{
    board_init();
    tusb_init();
}