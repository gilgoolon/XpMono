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

std::wstring Wireless::to_wstring(const Band band)
{
	switch (band)
	{
	case Band::GHZ_2_4:
		return L"2.4 GHz";
	case Band::GHZ_5:
		return L"5 GHz";
	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}

std::wstring Wireless::to_wstring(const Authentication authentication)
{
	switch (authentication)
	{
	DEFINE_ENUM_FORMAT_CASE(Authentication::OPEN);
	DEFINE_ENUM_FORMAT_CASE(Authentication::SHARED_KEY);
	DEFINE_ENUM_FORMAT_CASE(Authentication::WPA);
	DEFINE_ENUM_FORMAT_CASE(Authentication::WPA_PSK);
	DEFINE_ENUM_FORMAT_CASE(Authentication::WPA_NONE);
	DEFINE_ENUM_FORMAT_CASE(Authentication::RSNA);
	DEFINE_ENUM_FORMAT_CASE(Authentication::RSNA_PSK);
	DEFINE_ENUM_FORMAT_CASE(Authentication::WPA3);
	DEFINE_ENUM_FORMAT_CASE(Authentication::WPA3_SAE);
	DEFINE_ENUM_FORMAT_CASE(Authentication::OWE);
	DEFINE_ENUM_FORMAT_CASE(Authentication::WPA3_ENT);
	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}

std::wstring Wireless::to_wstring(const Encryption encryption)
{
	switch (encryption)
	{
	DEFINE_ENUM_FORMAT_CASE(Encryption::NONE);
	DEFINE_ENUM_FORMAT_CASE(Encryption::WEP40);
	DEFINE_ENUM_FORMAT_CASE(Encryption::TKIP);
	DEFINE_ENUM_FORMAT_CASE(Encryption::CCMP);
	DEFINE_ENUM_FORMAT_CASE(Encryption::WEP104);
	DEFINE_ENUM_FORMAT_CASE(Encryption::BIP);
	DEFINE_ENUM_FORMAT_CASE(Encryption::GCMP);
	DEFINE_ENUM_FORMAT_CASE(Encryption::GCMP_256);
	DEFINE_ENUM_FORMAT_CASE(Encryption::CCMP_256);
	DEFINE_ENUM_FORMAT_CASE(Encryption::BIP_GMAC_128);
	DEFINE_ENUM_FORMAT_CASE(Encryption::BIP_GMAC_256);
	DEFINE_ENUM_FORMAT_CASE(Encryption::BIP_CMAC_256);
	DEFINE_ENUM_FORMAT_CASE(Encryption::WPA_USE_GROUP);
	DEFINE_ENUM_FORMAT_CASE(Encryption::WEP);
	DEFINE_ENUM_FORMAT_CASE(Encryption::IHV_START);
	DEFINE_ENUM_FORMAT_CASE(Encryption::IHV_END);
	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}

Wireless::Band Wireless::frequency_to_band(const uint32_t frequency_mhz)
{
	static constexpr uint32_t MAX_2_4_FREQUENCY_MHZ = 3000;

	return frequency_mhz < MAX_2_4_FREQUENCY_MHZ ? Band::GHZ_2_4 : Band::GHZ_5;
}

std::wstring Wireless::ReducedNetwork::serialize() const
{
	const Formatting::Fields fields = {
		{L"ssid", details.ssid},
		{L"authentication", to_wstring(details.authentication)},
		{L"encryption", to_wstring(details.encryption)},
		{L"bssid", format_mac_address(station.bssid)},
		{L"signal_strength_dbm", Strings::to_wstring(station.signal_strength_dbm)},
		{L"signal_strength_percentage", Strings::to_wstring(station.signal_strength_percentage)},
		{L"frequency_mhz", Strings::to_wstring(station.frequency_mhz)},
		{L"band", to_wstring(station.band)},
	};

	return Formatting::format_fields(fields);
}

std::vector<Wireless::ReducedNetwork> Wireless::reduce(const Network& network)
{
	PhysicalStation closest_station = *std::ranges::max_element(
		network.stations,
		[](const PhysicalStation& station1, const PhysicalStation& station2)
		{
			return station1.signal_strength_dbm < station2.signal_strength_dbm;
		}
	);
	return {ReducedNetwork{.details = network.details, .station = std::move(closest_station)}};
}

std::vector<Wireless::ReducedNetwork> Wireless::expand(const Network& network)
{
	std::vector<ReducedNetwork> networks;
	std::ranges::transform(
		network.stations,
		std::back_inserter(networks),
		[network](const PhysicalStation& station)
		{
			return ReducedNetwork{.details = network.details, .station = station};
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
	static constexpr DOT11_BSS_TYPE STANDARD_NETWORKS = dot11_BSS_type_infrastructure;
	PWLAN_BSS_LIST bssid_list = nullptr;

	const DWORD result = WlanGetNetworkBssList(
		m_handle,
		inter.get_guid(),
		ssid,
		STANDARD_NETWORKS,
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

	for (uint32_t i = 0; i < bssid_list->dwNumberOfItems; ++i)
	{
		Buffer bssid = as_buffer(bssid_list->wlanBssEntries[i].dot11Bssid);
		const int32_t signal_strength = static_cast<int32_t>(bssid_list->wlanBssEntries[i].lRssi);
		const uint32_t signal_strength_percentage = bssid_list->wlanBssEntries[i].uLinkQuality;
		const uint32_t frequency_mhz = bssid_list->wlanBssEntries[i].ulChCenterFrequency / 1000;
		const Band band = frequency_to_band(frequency_mhz);

		stations.emplace_back(std::move(bssid), signal_strength, signal_strength_percentage, frequency_mhz, band);
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
		const auto authentication = static_cast<Authentication>(network.dot11DefaultAuthAlgorithm);
		const auto encryption = static_cast<Encryption>(network.dot11DefaultCipherAlgorithm);

		networks.emplace_back(
			NetworkDetails{
				.ssid = Strings::to_wstring(ssid),
				.authentication = authentication,
				.encryption = encryption,
			},
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
