﻿#include "ICommandHandler.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Products/ErrorProduct.hpp"

std::vector<IProduct::Ptr> ICommandHandler::handle(const ICommand::Ptr& command)
{
	std::vector<IProduct::Ptr> result;
	try
	{
		result = do_handle(command);
	}
	catch (const Exception& ex)
	{
		TRACE(L"caught exception with code ", ex.code(), " while handling, returning ErrorProduct");
		result.push_back(std::make_unique<ErrorProduct>(command, ex.code()));
	}
	catch (...)
	{
		TRACE(L"caught unknown exception while handling, returning ErrorProduct");
		result.push_back(std::make_unique<ErrorProduct>(command, ErrorCode::UNKNOWN_ERROR));
	}
	return result;
}
