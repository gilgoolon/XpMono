#include "SendKeyStrokesAction.hpp"

#include "bsp/board.h"
#include "tusb.h"

SendKeyStrokesAction::SendKeyStrokesAction(std::vector<KeyStroke> key_strokes):
    m_key_strokes(std::move(key_strokes))
{
}

void SendKeyStrokesAction::run()
{
    tud_task();
    for (const KeyStroke &keystroke : m_key_strokes)
    {
        keystroke.press();
        board_delay(8);
        keystroke.release();
        board_delay(8);
    }
    tud_task();
}