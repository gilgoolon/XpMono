#include "ICommandHandler.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Products/ErrorProduct.hpp"

std::vector<IProduct::Ptr> ICommandHandler::handle(ICommand::Ptr command)
{
	try
	{
		return do_handle(command);
	}
	catch (const Exception& ex)
	{
		TRACE(L"caught exception with code ", ex.code(), " while handling, returning ErrorProduct");
		return {std::make_unique<ErrorProduct>(command, ex.code())};
	}
}
