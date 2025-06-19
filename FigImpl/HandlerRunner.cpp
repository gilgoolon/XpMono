#include "HandlerRunner.hpp"

#include "Exception.hpp"
#include "FigImplException.hpp"

HandlerRunner::HandlerRunner(std::shared_ptr<IOperationHandler> handler):
	m_handler(std::move(handler))
{
}

void HandlerRunner::run()
{
	try
	{
		m_handler->run();
	}
	catch (const FigImplException& ex)
	{
		m_handler->error(ex.code(), ex.specific_code());
		throw;
	}
	catch (const Exception& ex)
	{
		m_handler->error(Fig::FigCode::FAILED_RUN, static_cast<Fig::FigSpecificCode>(ex.code()));
		throw;
	}
	catch (...)
	{
		m_handler->error(Fig::FigCode::FAILED_UNKNOWN, static_cast<Fig::FigSpecificCode>(ErrorCode::UNKNOWN_ERROR));
		throw;
	}
}
