#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include "KeyStroke.hpp"
#include "BoardRuntime.hpp"
#include "Actions/PayloadActionFactory.hpp"
#include "File.hpp"
#include "Strings.hpp"
#include "Config.hpp"
#include "Board.hpp"

void main_logic()
{
    const std::string raw_payload = Strings::as_string(File(Config::PAYLOAD_PATH).read_all()); 
    const std::string raw_payload = File(Config::PAYLOAD_PATH).read_text();
    Board::flash_led();
    const std::vector<IPayloadAction::Ptr> payload = PayloadActionFactory::make(raw_payload);

    for (const IPayloadAction::Ptr &action : payload)
    {
        action->run();
    }

    Board::flash_led();
}

int main(void)
{
    try
    {
        BoardRuntime runtime;
        Board::flash_led();
        main_logic();
        Board::flash_led();
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