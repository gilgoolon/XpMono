#pragma once
#include "pico/stdlib.h"

#include <vector>
#include <string>

using KeyCode = uint8_t;
using KeyModifier = uint8_t;
struct ModifiedKey final
{
    KeyCode code;
    KeyModifier modifier;
};

class KeyStroke final
{
public:
    explicit KeyStroke(std::vector<KeyCode> key_codes, KeyModifier key_modifier);
    virtual ~KeyStroke() = default;
    KeyStroke(const KeyStroke &) = delete;
    KeyStroke &operator=(const KeyStroke &) = delete;
    KeyStroke(KeyStroke &&) = default;
    KeyStroke &operator=(KeyStroke &&) = default;

public:
    void press() const;
    void release() const;

private:
    static constexpr uint32_t MAX_KEYS_PER_STROKE = 6;
    static void send_report(const std::vector<KeyCode> &key_codes, KeyModifier key_modifier);

private:
    std::vector<KeyCode> m_key_codes;
    KeyModifier m_key_modifier;

public:
    [[nodiscard]] static std::vector<KeyStroke> from_string(const std::string &string);
    [[nodiscard]] static ModifiedKey from_char(const char ch);
};