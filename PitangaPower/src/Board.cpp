#include "Board.hpp"

#include <bsp/board.h>

void Board::flash_led(uint32_t cooldown_millis)
{
    board_led_write(true);
    board_delay(1000);
    board_led_write(false);
    board_delay(1000);
}