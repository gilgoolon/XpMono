#include "DiscoverNetworksHandler.hpp"

#include "Trace.hpp"
#include "Networking/Wireless.hpp"
#include "Products/TextTypedProduct.hpp"

namespace Parameters
{
static constexpr auto REDUCE = "reduce";
static constexpr bool REDUCE_DEFAULT = false;
}

DiscoverNetworksHandler::DiscoverNetworksHandler(std::unique_ptr<Event> operation_event, const bool reduce):
	IOperationHandler(std::move(operation_event)),
	m_reduce(reduce)
{
}

DiscoverNetworksHandler::DiscoverNetworksHandler(std::unique_ptr<Event> operation_event, const Json& parameters):
	DiscoverNetworksHandler(
		std::move(operation_event),
		parameters.value(Parameters::REDUCE, Parameters::REDUCE_DEFAULT)
	)
{
}

DiscoverNetworksHandler::DiscoverNetworksHandler(std::unique_ptr<Event> operation_event, const Buffer& raw_parameters):
	DiscoverNetworksHandler(std::move(operation_event), Json::parse(Strings::to_string(raw_parameters)))
{
}

std::wstring DiscoverNetworksHandler::format_network(const Wireless::ReducedNetwork& network)
{
	return Strings::concat(
		std::wstring{L"#Network"},
		std::wstring{L"\n"},
		network.serialize()
	);
}

void DiscoverNetworksHandler::run()
{
	static constexpr auto SUFFIX = L"\n";
	std::wstring product;

	std::vector<Wireless::ReducedNetwork> networks;

	product.append(L"[Networks]\n");
	const auto operation = m_reduce ? Wireless::reduce : Wireless::expand;
	for (const auto& network : Wireless::enumerate_networks())
	{
		const std::vector<Wireless::ReducedNetwork> expanded = operation(network);
		networks.insert(networks.end(), expanded.begin(), expanded.end());
	}

	for (const Wireless::ReducedNetwork& network : networks)
	{
		product.append(format_network(network) + SUFFIX);
	}

	append(std::make_unique<TextTypedProduct>(product));
	finished();
}
