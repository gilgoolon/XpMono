#pragma once
#include "ICommand.hpp"
#include "IProduct.hpp"

#include <vector>

class Liver;

using LiverContext = Liver&;

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

	virtual std::vector<IProduct::Ptr> handle(LiverContext context) const = 0;
};
