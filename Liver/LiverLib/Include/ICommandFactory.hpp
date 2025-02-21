#pragma once
#include "ICommand.hpp"
#include "Utils/Buffer.hpp"

class ICommandFactory
{
public:
	explicit ICommandFactory() = default;
	virtual ~ICommandFactory() = default;
	ICommandFactory(const ICommandFactory&) = delete;
	ICommandFactory& operator=(const ICommandFactory&) = delete;
	ICommandFactory(ICommandFactory&&) = delete;
	ICommandFactory& operator=(ICommandFactory&&) = delete;

	[[nodiscard]] virtual ICommand::Ptr create(const Buffer& command);
};
