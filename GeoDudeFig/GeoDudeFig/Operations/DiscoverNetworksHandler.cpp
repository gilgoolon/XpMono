#include "DiscoverNetworksHandler.hpp"

#include "SerializableSection.hpp"
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
	DiscoverNetworksHandler(
		std::move(operation_event),
		raw_parameters.empty() ? Json::object() : Json::parse(Strings::to_string(raw_parameters))
	)
{
}

void DiscoverNetworksHandler::run()
{
	std::vector<std::unique_ptr<Wireless::ReducedNetwork>> networks;

	SerializableSection networks_section{.name = L"Networks", .objects = {}};

	const auto operation = m_reduce ? Wireless::reduce : Wireless::expand;
	for (const auto& network : Wireless::enumerate_networks())
	{
		std::vector<std::unique_ptr<Wireless::ReducedNetwork>> expanded = operation(network);
		networks.insert(
			networks.end(),
			std::make_move_iterator(expanded.begin()),
			std::make_move_iterator(expanded.end())
		);
	}

	for (std::unique_ptr<Wireless::ReducedNetwork>& network : networks)
	{
		networks_section.objects.push_back(std::move(network));
	}

	append(std::make_unique<TextTypedProduct>(networks_section.serialize()));
	finished();
}
