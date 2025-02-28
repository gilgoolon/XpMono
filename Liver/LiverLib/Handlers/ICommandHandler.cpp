#include "ICommandHandler.hpp"

#include "Exception.hpp"

std::vector<IProduct::Ptr> ICommandHandler::run_handler(LiverContext& context, Ptr handler)
{
	try
	{
		return handler->handle(context);
	}
	catch (const Exception& ex)
	{
	}
}
