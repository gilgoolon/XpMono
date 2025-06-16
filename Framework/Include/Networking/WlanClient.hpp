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

	const GUID* get_guid() const;
};

struct PhysicalStation final
{
	Buffer bssid;
	int32_t signal_strength_db;
};

struct Network final
{
	std::wstring ssid;
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
