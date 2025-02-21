#pragma once
#include "ICommandHandler.hpp"

namespace CommandHandlerFactory
{
[[nodiscard]] ICommandHandler::Ptr create(std::shared_ptr<ICommand> command);
};
