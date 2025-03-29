#include "DelayAction.hpp"

#include "bsp/board.h"

DelayAction::DelayAction(const uint32_t delay_millis):
    m_delay_millis(delay_millis)
{
}

void DelayAction::run()
{
    board_delay(m_delay_millis);
}