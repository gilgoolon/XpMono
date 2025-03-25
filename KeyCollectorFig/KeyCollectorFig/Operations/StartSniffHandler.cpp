#include "StartSniffHandler.hpp"

#include "ActiveKeySniff.hpp"

void StartSniffHandler::run()
{
	ActiveKeySniff::start();

	finished();
}
