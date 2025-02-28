#include "ICommandHandler.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Products/ErrorProduct.hpp"

std::vector<IProduct::Ptr> ICommandHandler::run_handler(LiverContext& context, const Ptr& handler)
{
	try
	{
		return handler->handle(context);
	}
	catch (const Exception& ex)
	{
		TRACE(L"caught exception with code ", ex.code(), " while handling, returning ErrorProduct");
		return {std::make_unique<ErrorProduct>(handler->m_command, ex.code())};
	}
}
