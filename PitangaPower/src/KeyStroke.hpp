#pragma once
#include "pico/stdlib.h"

#include <vector>
#include <string>

struct KeyCode final
{
    uint8_t key_code;
    uint8_t modifier;
};

class KeyStroke final
{
public:
    explicit KeyStroke(std::vector<KeyCode> key_codes);
    virtual ~KeyStroke() = default;
    KeyStroke(const KeyStroke&) = delete;
    KeyStroke& operator=(const KeyStroke&) = delete;
    KeyStroke(KeyStroke&&) = default;
    KeyStroke& operator=(KeyStroke&&) = default;

public:
    void press() const;
    void release() const;
    
private:
    static constexpr uint32_t MAX_KEYS_PER_STROKE = 6;
    static void send_report(const std::vector<KeyCode>& key_codes);

private:
    std::vector<KeyCode> m_key_codes;

public:
    [[nodiscard]] static std::vector<KeyStroke> from_string(const std::string& string);
    [[nodiscard]] static KeyCode from_char(const char ch);
};