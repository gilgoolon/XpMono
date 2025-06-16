#pragma once

#include "Utils/Buffer.hpp"

#include <string>
#include <Windows.h>
#include <WlanTypes.h>

namespace Wireless
{
struct Interface final
{
	Buffer guid;
	std::wstring description;

	[[nodiscard]] const GUID* get_guid() const;
};

enum class Band : uint8_t
{
	GHZ_5 = 0,
	GHZ_2_4,
};

[[nodiscard]] Band frequency_to_band(uint32_t frequency_mhz);

struct PhysicalStation final
{
	Buffer bssid;
	int32_t signal_strength_dbm;
	uint32_t signal_strength_percentage;
	uint32_t frequency_mhz;
	Band band;
};

enum class Authentication
{
	OPEN = DOT11_AUTH_ALGO_80211_OPEN,
	SHARED_KEY = DOT11_AUTH_ALGO_80211_SHARED_KEY,
	WPA = DOT11_AUTH_ALGO_WPA,
	WPA_PSK = DOT11_AUTH_ALGO_WPA_PSK,
	WPA_NONE = DOT11_AUTH_ALGO_WPA_NONE,
	RSNA = DOT11_AUTH_ALGO_RSNA,
	RSNA_PSK = DOT11_AUTH_ALGO_RSNA_PSK,
	WPA3 = DOT11_AUTH_ALGO_WPA3,
	WPA3_SAE = DOT11_AUTH_ALGO_WPA3_SAE,
	OWE = DOT11_AUTH_ALGO_OWE,
	WPA3_ENT = DOT11_AUTH_ALGO_WPA3_ENT,
};

enum class Encryption
{
	NONE = DOT11_CIPHER_ALGO_NONE,
	WEP40 = DOT11_CIPHER_ALGO_WEP40,
	TKIP = DOT11_CIPHER_ALGO_TKIP,
	CCMP = DOT11_CIPHER_ALGO_CCMP,
	WEP104 = DOT11_CIPHER_ALGO_WEP104,
	BIP = DOT11_CIPHER_ALGO_BIP,
	GCMP = DOT11_CIPHER_ALGO_GCMP,
	GCMP_256 = DOT11_CIPHER_ALGO_GCMP_256,
	CCMP_256 = DOT11_CIPHER_ALGO_CCMP_256,
	BIP_GMAC_128 = DOT11_CIPHER_ALGO_BIP_GMAC_128,
	BIP_GMAC_256 = DOT11_CIPHER_ALGO_BIP_GMAC_256,
	BIP_CMAC_256 = DOT11_CIPHER_ALGO_BIP_CMAC_256,
	WPA_USE_GROUP = DOT11_CIPHER_ALGO_WPA_USE_GROUP,
	RSN_USE_GROUP = DOT11_CIPHER_ALGO_RSN_USE_GROUP,
	WEP = DOT11_CIPHER_ALGO_WEP,
	IHV_START = DOT11_CIPHER_ALGO_IHV_START,
	IHV_END = DOT11_CIPHER_ALGO_IHV_END,
};

struct Network final
{
	std::wstring ssid;
	Authentication authentication;
	Encryption encryption;
	std::vector<PhysicalStation> stations;
};

struct ReducedNetwork final
{
	std::wstring ssid;
	PhysicalStation station;

	[[nodiscard]] std::wstring serialize() const;
};

std::vector<ReducedNetwork> reduce(const Network& network);
std::vector<ReducedNetwork> expand(const Network& network);

class WlanClient final
{
public:
	explicit WlanClient();
	~WlanClient();
	WlanClient(const WlanClient&) = delete;
	WlanClient& operator=(const WlanClient&) = delete;
	WlanClient(WlanClient&&) = delete;
	WlanClient& operator=(WlanClient&&) = delete;

private:
	[[nodiscard]] static HANDLE create_client();

	[[nodiscard]] std::vector<PhysicalStation> enumerate_stations_for_network(
		const Interface& inter,
		PDOT11_SSID ssid,
		BOOL security_enabled) const;

public:
	[[nodiscard]] std::vector<Interface> enumerate_interfaces() const;
	[[nodiscard]] std::vector<Network> enumerate_networks_for_interface(const Interface& inter) const;

private:
	HANDLE m_handle;
};

[[nodiscard]] std::wstring format_mac_address(const Buffer& mac_address);
}
