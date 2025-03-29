#pragma once
#include <memory>

class IPayloadAction
{
public:
    using Ptr = std::unique_ptr<IPayloadAction>;

    explicit IPayloadAction() = default;
    virtual ~IPayloadAction() = default;
    IPayloadAction(const IPayloadAction&) = delete;
    IPayloadAction& operator=(const IPayloadAction&) = delete;
    IPayloadAction(IPayloadAction&&) = delete;
    IPayloadAction& operator=(IPayloadAction&&) = delete;

public:
    virtual void run() = 0;
};
