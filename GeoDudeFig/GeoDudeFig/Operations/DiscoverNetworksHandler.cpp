#include "DiscoverNetworksHandler.hpp"

#include "Trace.hpp"
#include "Products/TextTypedProduct.hpp"

DiscoverNetworksHandler::DiscoverNetworksHandler(std::unique_ptr<Event> operation_event):
	IOperationHandler(std::move(operation_event))
{
}

void DiscoverNetworksHandler::run()
{
	static constexpr auto SUFFIX = L"\n";
	std::wstring product;

	const std::vector<Wireless::NetworkInfo> networks = Wireless::enumerate_networks();
	for (size_t i = 0; i < networks.size(); ++i)
	{
		product.append(format_network(networks[i], i) + SUFFIX);
	}

	append(std::make_unique<TextTypedProduct>(product));
	finished();
}
