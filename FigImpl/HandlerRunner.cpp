#include "HandlerRunner.hpp"

HandlerRunner::HandlerRunner(std::shared_ptr<IOperationHandler> handler):
	m_handler(std::move(handler))
{
}

void HandlerRunner::run()
{
	m_handler->run();
}
