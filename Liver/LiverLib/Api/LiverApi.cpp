#include "Include/LiverApi.hpp"

#include "Exception.hpp"
#include "Liver.hpp"
#include "Trace.hpp"

void LiverApi::main(const Buffer& configuration)
{
	try
	{
		const std::unique_ptr<Liver> liver = Liver::create(configuration);
		liver->run();
	}
	CATCH_AND_TRACE()
}
