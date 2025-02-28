#pragma once
#include "Commands/ICommand.hpp"
#include "Communicators/Protocol/Command.hpp"

class ICommandFactory
{
public:
	using Ptr = std::unique_ptr<ICommandFactory>;

	explicit ICommandFactory() = default;
	virtual ~ICommandFactory() = default;
	ICommandFactory(const ICommandFactory&) = delete;
	ICommandFactory& operator=(const ICommandFactory&) = delete;
	ICommandFactory(ICommandFactory&&) = delete;
	ICommandFactory& operator=(ICommandFactory&&) = delete;

	[[nodiscard]] virtual ICommand::Ptr create(const Command& command) = 0;
};
