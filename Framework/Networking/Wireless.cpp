#include "Networking/Wireless.hpp"

#include "Networking/WlanClient.hpp"

std::vector<Wireless::Interface> Wireless::enumerate_interfaces()
{
	return WlanClient().enumerate_interfaces();
}

std::vector<Wireless::Network> Wireless::enumerate_networks()
{
	std::vector<Network> networks;

	for (const WlanClient client; const Interface& inter : client.enumerate_interfaces())
	{
		std::vector<Network> interface_networks = client.enumerate_networks_for_interface(inter);
		networks.insert(networks.end(), interface_networks.begin(), interface_networks.end());
	}

	return networks;
}
