#include "Networking/WlanClient.hpp"

#include "Exception.hpp"
#include "Trace.hpp"
#include "Networking/ScopedWlanMemory.hpp"
#include "Utils/Strings.hpp"

#include <functional>
#include <wlanapi.h>

const GUID* Wireless::Interface::get_guid() const
{
	return reinterpret_cast<const GUID*>(guid.data());
}

std::wstring Wireless::ReducedNetwork::serialize() const
{
	const Formatting::Fields fields = {
		{L"ssid", ssid},
		{L"bssid", format_mac_address(station.bssid)},
		{L"bssid", Strings::to_wstring(station.signal_strength_db)},
	};

	return Formatting::format_fields(fields);
}

std::vector<Wireless::ReducedNetwork> Wireless::reduce(const Network& network)
{
	PhysicalStation closest_station = *std::ranges::max_element(
		network.stations,
		[](const PhysicalStation& station1, const PhysicalStation& station2)
		{
			return station1.signal_strength_db < station2.signal_strength_db;
		}
	);
	return {ReducedNetwork{.ssid = network.ssid, .station = std::move(closest_station)}};
}

std::vector<Wireless::ReducedNetwork> Wireless::expand(const Network& network)
{
	std::vector<ReducedNetwork> networks;
	std::ranges::transform(
		network.stations,
		std::back_inserter(networks),
		[network](const PhysicalStation& station)
		{
			return ReducedNetwork{.ssid = network.ssid, .station = station};
		}
	);
	return networks;
}

Wireless::WlanClient::WlanClient():
	m_handle(create_client())
{
}

Wireless::WlanClient::~WlanClient()
{
	try
	{
		static constexpr PVOID RESERVED = nullptr;
		const DWORD result = WlanCloseHandle(m_handle, RESERVED);
		if (result != ERROR_SUCCESS)
		{
			TRACE(L"failed to close wlan handle with error ", result);
		}
	}
	catch (...)
	{
		TRACE(L"failed to close wlan handle");
	}
}

HANDLE Wireless::WlanClient::create_client()
{
	static constexpr DWORD MODERN_WINDOWS_VERSION = 2;
	static constexpr PVOID RESERVED = nullptr;

	DWORD negotiated_version = 0;
	HANDLE client_handle = nullptr;

	const DWORD result = WlanOpenHandle(MODERN_WINDOWS_VERSION, RESERVED, &negotiated_version, &client_handle);
	if (result != ERROR_SUCCESS)
	{
		throw WinApiException(ErrorCode::FAILED_WLAN_CREATE_CLIENT, result);
	}

	return client_handle;
}

std::vector<Wireless::PhysicalStation> Wireless::WlanClient::enumerate_stations_for_network(
	const Interface& inter,
	const PDOT11_SSID ssid,
	const BOOL security_enabled) const
{
	static constexpr PVOID RESERVED = nullptr;
	static constexpr DOT11_BSS_TYPE ANY_BSSID = dot11_BSS_type_any;
	PWLAN_BSS_LIST bssid_list = nullptr;

	const DWORD result = WlanGetNetworkBssList(
		m_handle,
		inter.get_guid(),
		ssid,
		ANY_BSSID,
		security_enabled,
		RESERVED,
		&bssid_list
	);

	if (result != ERROR_SUCCESS)
	{
		throw WinApiException(ErrorCode::FAILED_WLAN_ENUMERATE_BSSIDS, result);
	}

	ScopedWlanMemory bssid_list_guard(bssid_list);

	std::vector<PhysicalStation> stations;

	for (uint32_t i = 0; i < bssid_list->dwTotalSize; ++i)
	{
		Buffer bssid = as_buffer(bssid_list->wlanBssEntries[i].dot11Bssid);
		int32_t signal_strength = static_cast<int32_t>(bssid_list->wlanBssEntries[i].lRssi);

		stations.emplace_back(bssid, signal_strength);
	}

	return stations;
}

std::vector<Wireless::Interface> Wireless::WlanClient::enumerate_interfaces() const
{
	static constexpr PVOID RESERVED = nullptr;
	PWLAN_INTERFACE_INFO_LIST interfaces_list = nullptr;

	const DWORD result = WlanEnumInterfaces(m_handle, RESERVED, &interfaces_list);

	if (result != ERROR_SUCCESS)
	{
		throw WinApiException(ErrorCode::FAILED_WLAN_ENUMERATE_INTERFACES, result);
	}

	ScopedWlanMemory interfaces_list_guard(interfaces_list);

	std::vector<Interface> interfaces;

	for (uint32_t i = 0; i < interfaces_list->dwNumberOfItems; ++i)
	{
		Buffer interface_guid = as_buffer(interfaces_list->InterfaceInfo[i].InterfaceGuid);
		std::wstring interface_description = interfaces_list->InterfaceInfo[i].strInterfaceDescription;

		interfaces.emplace_back(interface_guid, interface_description);
	}

	return interfaces;
}

std::vector<Wireless::Network> Wireless::WlanClient::enumerate_networks_for_interface(const Interface& inter) const
{
	static constexpr PVOID RESERVED = nullptr;
	static constexpr DWORD SHOW_HIDDEN_NETWORKS = WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES |
		WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_MANUAL_HIDDEN_PROFILES;
	PWLAN_AVAILABLE_NETWORK_LIST network_list = nullptr;

	const DWORD result = WlanGetAvailableNetworkList(
		m_handle,
		inter.get_guid(),
		SHOW_HIDDEN_NETWORKS,
		RESERVED,
		&network_list
	);

	if (result != ERROR_SUCCESS)
	{
		throw WinApiException(ErrorCode::FAILED_WLAN_ENUMERATE_NETWORKS, result);
	}

	ScopedWlanMemory interfaces_list_guard(network_list);

	std::vector<Network> networks;

	for (uint32_t i = 0; i < network_list->dwNumberOfItems; ++i)
	{
		WLAN_AVAILABLE_NETWORK& network = network_list->Network[i];

		std::string ssid{network.dot11Ssid.ucSSID, network.dot11Ssid.ucSSID + network.dot11Ssid.uSSIDLength};

		networks.emplace_back(
			Strings::to_wstring(ssid),
			enumerate_stations_for_network(inter, &network.dot11Ssid, network.bSecurityEnabled)
		);
	}

	return networks;
}

static std::wstring format_hex_byte(const uint8_t value)
{
	return Formatting::format_hex(value);
}

std::wstring Wireless::format_mac_address(const Buffer& mac_address)
{
	static constexpr size_t EXPECTED_MAC_ADDRESS_SIZE = 6;

	if (mac_address.size() != EXPECTED_MAC_ADDRESS_SIZE)
	{
		throw Exception(ErrorCode::INVALID_ARGUMENT);
	}

	std::vector<std::wstring> bytes;
	std::ranges::transform(
		mac_address,
		std::back_inserter(bytes),
		format_hex_byte
	);
	return Strings::join(bytes, L':');
}
