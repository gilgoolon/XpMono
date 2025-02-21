#include "CommandHandlerFactory.hpp"

ICommandHandler::Ptr CommandHandlerFactory::create(std::shared_ptr<ICommand> command)
{
	return nullptr;
}
