#pragma once
#include "IPayloadAction.hpp"

class DelayAction : public IPayloadAction
{
public:
    explicit DelayAction(uint32_t delay_millis);
    virtual ~DelayAction() = default;
    DelayAction(const DelayAction &) = delete;
    DelayAction &operator=(const DelayAction &) = delete;
    DelayAction(DelayAction &&) = delete;
    DelayAction &operator=(DelayAction &&) = delete;

public:
    void run() override;

private:
    uint32_t m_delay_millis;
};
