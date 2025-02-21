#pragma once
#include "ICommandHandler.hpp"

#include <memory>

class ICommandHandlerFactory
{
public:
	using Ptr = std::unique_ptr<ICommandHandlerFactory>;

	explicit ICommandHandlerFactory() = default;
	virtual ~ICommandHandlerFactory() = default;
	ICommandHandlerFactory(const ICommandHandlerFactory&) = delete;
	ICommandHandlerFactory& operator=(const ICommandHandlerFactory&) = delete;
	ICommandHandlerFactory(ICommandHandlerFactory&&) = delete;
	ICommandHandlerFactory& operator=(ICommandHandlerFactory&&) = delete;

	[[nodiscard]] virtual ICommandHandler::Ptr create(std::shared_ptr<ICommand> command) const = 0;
};
