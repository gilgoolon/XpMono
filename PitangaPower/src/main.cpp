#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

void send_keystroke(uint8_t key);

int main(void)
{
    board_init();
    tusb_init();

    while (1)
    {
        tud_task(); // TinyUSB device task

        board_led_write(true);
        board_delay(5000);
        board_led_write(false);

        board_led_write(true);

        send_keystroke(HID_KEY_A);
        board_delay(50);
        send_keystroke(0);

        board_led_write(false);
        board_delay(500);
    }

    return 0;
}

void send_keystroke(uint8_t key)
{
    if (!tud_hid_ready())
        return;
    uint8_t keycode[6] = {0};
    keycode[0] = key;

    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
}

// HID Get Report Callback (Host requests a report from the device)
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    // This function is called when the host requests a report (e.g., keyboard status, feature report)
    // Since we don't have a custom report, return 0
    return 0;
}

// HID Set Report Callback (Host sends a report to the device)
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    // This function is called when the host sends an HID output or feature report
    // Commonly used for LED control (Caps Lock, Num Lock, etc.)

    if (report_type == HID_REPORT_TYPE_OUTPUT && bufsize > 0)
    {
        // Handle LED status from host (e.g., Caps Lock)
        uint8_t led_status = buffer[0];

        if (led_status & KEYBOARD_LED_CAPSLOCK)
        {
            board_led_write(true); // Turn on an LED if Caps Lock is active
        }
        else
        {
            board_led_write(false); // Turn off LED
        }
    }
}