#pragma once

#include "IPayloadAction.hpp"
#include "Json.hpp"

#include <vector>
#include <string>

namespace PayloadActionFactory
{
    std::vector<IPayloadAction::Ptr> make(const std::string &payload);
    IPayloadAction::Ptr make_action(const Json &payload);
    
}
