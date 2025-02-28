#pragma once
#include "IProduct.hpp"
#include "Commands/ICommand.hpp"

#include <vector>

class Liver;

class ICommandHandler
{
public:
	using Ptr = std::unique_ptr<ICommandHandler>;

	explicit ICommandHandler() = default;
	virtual ~ICommandHandler() = default;
	ICommandHandler(const ICommandHandler&) = delete;
	ICommandHandler& operator=(const ICommandHandler&) = delete;
	ICommandHandler(ICommandHandler&&) = delete;
	ICommandHandler& operator=(ICommandHandler&&) = delete;

	std::vector<IProduct::Ptr> handle(ICommand::Ptr command);
	virtual std::vector<IProduct::Ptr> do_handle(ICommand::Ptr) = 0;
};
