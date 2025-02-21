#pragma once
#include "ICommandHandler.hpp"

namespace CommandHandlerFactory
{
[[nodiscard]] ICommandHandler::Ptr create(ICommand::Ptr command);
};
