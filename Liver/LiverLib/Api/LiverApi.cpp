#include "Include/LiverApi.hpp"

#include "Liver.hpp"

void LiverApi::main(const Buffer& configuration)
{
	const std::unique_ptr<Liver> liver = Liver::create(configuration);
	liver->run();
}
