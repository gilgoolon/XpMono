#include "CommandHandlerFactory.hpp"

#include "Exception.hpp"
#include "Handlers/LoadDllHandler.hpp"

ICommandHandler::Ptr CommandHandlerFactory::create(ICommand::Ptr command)
{
	switch (command->type())
	{
	case ICommand::Type::LOAD_DLL:
		return std::make_unique<LoadDllHandler>(std::move(command));

	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}
