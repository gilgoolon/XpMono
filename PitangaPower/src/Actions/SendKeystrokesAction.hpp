#pragma once
#include "IPayloadAction.hpp"
#include "KeyStroke.hpp"

#include <vector>

class SendKeyStrokesAction final : public IPayloadAction
{
public:
    explicit SendKeyStrokesAction(std::vector<KeyStroke> key_strokes);
    ~SendKeyStrokesAction() override = default;
    SendKeyStrokesAction(const SendKeyStrokesAction &) = delete;
    SendKeyStrokesAction &operator=(const SendKeyStrokesAction &) = delete;
    SendKeyStrokesAction(SendKeyStrokesAction &&) = delete;
    SendKeyStrokesAction &operator=(SendKeyStrokesAction &&) = delete;

public:
    void run() override;

private:
    std::vector<KeyStroke> m_key_strokes;
};
