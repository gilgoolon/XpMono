#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include "KeyStroke.hpp"
#include "BoardRuntime.hpp"

void send_keystroke(uint8_t key);

void loop()
{
    std::vector<KeyStroke> payload = KeyStroke::from_string("hello world hello world hello world hello world hello world hello world");

    for (uint32_t i = 0; i < 5; ++i)
    {
        board_led_write(true);
        board_delay(500);
        board_led_write(false);
        board_delay(500);
    }
    for (const KeyStroke& keystroke : payload)
    {
        tud_task();

        keystroke.press();
        board_delay(50);
        keystroke.release();
    }
}

int main(void)
{
    try
    {
        BoardRuntime runtime;
        loop();
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// HID Get Report Callback (Host requests a report from the device)
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    static constexpr uint16_t DEFAULT_REPORT_NO_INFO = 0;
    return DEFAULT_REPORT_NO_INFO;
}

// HID Set Report Callback (Host sends a report to the device)
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    // Ignore host features
}