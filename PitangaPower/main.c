#include "pico/stdlib.h"
#include "tusb.h"

// Simple USB HID keyboard implementation
void send_keystroke(char key) {
    uint8_t report[6] = {0}; // HID report buffer
    report[2] = key;         // Set keycode

    tud_hid_keyboard_report(0, 0, report);
    sleep_ms(10);
    
    report[2] = 0; // Release key
    tud_hid_keyboard_report(0, 0, report);
}

int main() {
    stdio_init_all();
    tusb_init();

    while (1) {
        if (tud_hid_ready()) {
            send_keystroke(0x04); // 'A' key
            sleep_ms(1000);
        }
    }
}
