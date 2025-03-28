#include "KeyStroke.hpp"

#include "tusb.h"
#include "usb_descriptors.h"

#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

KeyStroke::KeyStroke(std::vector<KeyCode> key_codes) : m_key_codes(std::move(key_codes))
{
}

void KeyStroke::press() const
{
    send_report(m_key_codes);
}

void KeyStroke::release() const
{
    send_report({});
}

void KeyStroke::send_report(const std::vector<KeyCode> &key_codes)
{
    if (key_codes.size() > MAX_KEYS_PER_STROKE)
    {
        throw std::invalid_argument("too many keys for report...");
    }

    if (!tud_hid_ready())
    {
        return;
    }

    uint8_t pressed_keys[MAX_KEYS_PER_STROKE] = {0};
    static constexpr void *COPY_FAILED = nullptr;
    if (memcpy(pressed_keys, key_codes.data(), key_codes.size()) == COPY_FAILED)
    {
        throw std::runtime_error("failed to send hid keyboard report");
    }

    static constexpr uint8_t NO_MODIFIER = 0;
    static constexpr uint8_t* RELEASE_KEYS = nullptr;
    if (!tud_hid_keyboard_report(REPORT_ID_KEYBOARD, NO_MODIFIER, key_codes.empty() ? RELEASE_KEYS : pressed_keys))
    {
        throw std::runtime_error("failed to send hid keyboard report");
    }
}

static uint32_t shortest_unique_prefix_length(const std::string &string)
{
    std::unordered_set<char> set;
    for (uint32_t i = 0; i < string.size(); ++i)
    {
        if (set.contains(string[i]))
        {
            return i;
        }
        set.insert(string[i]);
    }
    return string.size();
}

    std::vector<std::unique_ptr<KeyStroke>> KeyStroke::from_string(const std::string &string)
{
    std::vector<std::unique_ptr<KeyStroke>> result;

    std::string characters_left = string;

    while (!characters_left.empty())
    {
        static constexpr uint32_t FROM_START = 0;
        const std::string string_to_send = characters_left.substr(FROM_START, (std::min)(MAX_KEYS_PER_STROKE, shortest_unique_prefix_length(characters_left)));
        std::vector<KeyCode> key_codes;
        for (const char ch : string_to_send)
        {
            key_codes.push_back(from_char(ch));
        }
        result.emplace_back(std::make_unique<KeyStroke>(std::move(key_codes)));

        characters_left = characters_left.substr(string_to_send.size());
    }

    return result;
}

KeyCode KeyStroke::from_char(const char ch)
{
    static constexpr uint8_t NO_MODIFIER = 0;
    static constexpr uint8_t SHIFT_MODIFIER = KEYBOARD_MODIFIER_LEFTSHIFT;
    static const std::unordered_map<char, KeyCode> CHAR_TO_KEY_CODE = {
        {'a', {HID_KEY_A, NO_MODIFIER}},
        {'b', {HID_KEY_B, NO_MODIFIER}},
        {'c', {HID_KEY_C, NO_MODIFIER}},
        {'d', {HID_KEY_D, NO_MODIFIER}},
        {'e', {HID_KEY_E, NO_MODIFIER}},
        {'f', {HID_KEY_F, NO_MODIFIER}},
        {'g', {HID_KEY_G, NO_MODIFIER}},
        {'h', {HID_KEY_H, NO_MODIFIER}},
        {'i', {HID_KEY_I, NO_MODIFIER}},
        {'j', {HID_KEY_J, NO_MODIFIER}},
        {'k', {HID_KEY_K, NO_MODIFIER}},
        {'l', {HID_KEY_L, NO_MODIFIER}},
        {'m', {HID_KEY_M, NO_MODIFIER}},
        {'n', {HID_KEY_N, NO_MODIFIER}},
        {'o', {HID_KEY_O, NO_MODIFIER}},
        {'p', {HID_KEY_P, NO_MODIFIER}},
        {'q', {HID_KEY_Q, NO_MODIFIER}},
        {'r', {HID_KEY_R, NO_MODIFIER}},
        {'s', {HID_KEY_S, NO_MODIFIER}},
        {'t', {HID_KEY_T, NO_MODIFIER}},
        {'u', {HID_KEY_U, NO_MODIFIER}},
        {'v', {HID_KEY_V, NO_MODIFIER}},
        {'w', {HID_KEY_W, NO_MODIFIER}},
        {'x', {HID_KEY_X, NO_MODIFIER}},
        {'y', {HID_KEY_Y, NO_MODIFIER}},
        {'z', {HID_KEY_Z, NO_MODIFIER}},

        {'A', {HID_KEY_A, SHIFT_MODIFIER}},
        {'B', {HID_KEY_B, SHIFT_MODIFIER}},
        {'C', {HID_KEY_C, SHIFT_MODIFIER}},
        {'D', {HID_KEY_D, SHIFT_MODIFIER}},
        {'E', {HID_KEY_E, SHIFT_MODIFIER}},
        {'F', {HID_KEY_F, SHIFT_MODIFIER}},
        {'G', {HID_KEY_G, SHIFT_MODIFIER}},
        {'H', {HID_KEY_H, SHIFT_MODIFIER}},
        {'I', {HID_KEY_I, SHIFT_MODIFIER}},
        {'J', {HID_KEY_J, SHIFT_MODIFIER}},
        {'K', {HID_KEY_K, SHIFT_MODIFIER}},
        {'L', {HID_KEY_L, SHIFT_MODIFIER}},
        {'M', {HID_KEY_M, SHIFT_MODIFIER}},
        {'N', {HID_KEY_N, SHIFT_MODIFIER}},
        {'O', {HID_KEY_O, SHIFT_MODIFIER}},
        {'P', {HID_KEY_P, SHIFT_MODIFIER}},
        {'Q', {HID_KEY_Q, SHIFT_MODIFIER}},
        {'R', {HID_KEY_R, SHIFT_MODIFIER}},
        {'S', {HID_KEY_S, SHIFT_MODIFIER}},
        {'T', {HID_KEY_T, SHIFT_MODIFIER}},
        {'U', {HID_KEY_U, SHIFT_MODIFIER}},
        {'V', {HID_KEY_V, SHIFT_MODIFIER}},
        {'W', {HID_KEY_W, SHIFT_MODIFIER}},
        {'X', {HID_KEY_X, SHIFT_MODIFIER}},
        {'Y', {HID_KEY_Y, SHIFT_MODIFIER}},
        {'Z', {HID_KEY_Z, SHIFT_MODIFIER}},

        {'1', {HID_KEY_1, NO_MODIFIER}},
        {'2', {HID_KEY_2, NO_MODIFIER}},
        {'3', {HID_KEY_3, NO_MODIFIER}},
        {'4', {HID_KEY_4, NO_MODIFIER}},
        {'5', {HID_KEY_5, NO_MODIFIER}},
        {'6', {HID_KEY_6, NO_MODIFIER}},
        {'7', {HID_KEY_7, NO_MODIFIER}},
        {'8', {HID_KEY_8, NO_MODIFIER}},
        {'9', {HID_KEY_9, NO_MODIFIER}},
        {'0', {HID_KEY_0, NO_MODIFIER}},

        {' ', {HID_KEY_SPACE, NO_MODIFIER}},
        {'.', {HID_KEY_PERIOD, NO_MODIFIER}},
        {',', {HID_KEY_COMMA, NO_MODIFIER}},
        {'/', {HID_KEY_SLASH, NO_MODIFIER}},
        {';', {HID_KEY_SEMICOLON, NO_MODIFIER}},
        {'\'', {HID_KEY_APOSTROPHE, NO_MODIFIER}},
        {'-', {HID_KEY_MINUS, NO_MODIFIER}},
        {'=', {HID_KEY_EQUAL, NO_MODIFIER}},
        {'[', {HID_KEY_BRACKET_LEFT, NO_MODIFIER}},
        {']', {HID_KEY_BRACKET_RIGHT, NO_MODIFIER}},
        {'\\', {HID_KEY_BACKSLASH, NO_MODIFIER}},

        {'!', {HID_KEY_1, SHIFT_MODIFIER}},
        {'@', {HID_KEY_2, SHIFT_MODIFIER}},
        {'#', {HID_KEY_3, SHIFT_MODIFIER}},
        {'$', {HID_KEY_4, SHIFT_MODIFIER}},
        {'%', {HID_KEY_5, SHIFT_MODIFIER}},
        {'^', {HID_KEY_6, SHIFT_MODIFIER}},
        {'&', {HID_KEY_7, SHIFT_MODIFIER}},
        {'*', {HID_KEY_8, SHIFT_MODIFIER}},
        {'(', {HID_KEY_9, SHIFT_MODIFIER}},
        {')', {HID_KEY_0, SHIFT_MODIFIER}},

        {'_', {HID_KEY_MINUS, SHIFT_MODIFIER}},
        {'+', {HID_KEY_EQUAL, SHIFT_MODIFIER}},
        {'{', {HID_KEY_BRACKET_LEFT, SHIFT_MODIFIER}},
        {'}', {HID_KEY_BRACKET_RIGHT, SHIFT_MODIFIER}},
        {'|', {HID_KEY_MINUS, SHIFT_MODIFIER}},
        {'"', {HID_KEY_APOSTROPHE, SHIFT_MODIFIER}},
        {'<', {HID_KEY_COMMA, SHIFT_MODIFIER}},
        {'>', {HID_KEY_PERIOD, SHIFT_MODIFIER}},
        {'?', {HID_KEY_SLASH, SHIFT_MODIFIER}},
    };

    return CHAR_TO_KEY_CODE.at(ch);
}
