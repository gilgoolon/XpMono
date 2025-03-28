#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include "KeyStroke.hpp"
#include "BoardRuntime.hpp"

void send_keystroke(uint8_t key);

void flash_led()
{
    board_led_write(true);
    board_delay(1000);
    board_led_write(false);
    board_delay(1000);
}

void loop()
{
    std::vector <KeyStroke> payload = KeyStroke::from_string("Hello World!");

    flash_led();
    for (const KeyStroke &keystroke : payload)
    {
        tud_task();
        keystroke.press();
        board_delay(50);
        keystroke.release();
        board_delay(50);
    }
    flash_led();
}

int main(void)
{
    try
    {
        BoardRuntime runtime;
        flash_led();
        loop();
        flash_led();
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