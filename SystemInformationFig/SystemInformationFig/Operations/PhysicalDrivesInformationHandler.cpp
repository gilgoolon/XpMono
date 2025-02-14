#include "PhysicalDrivesInformationHandler.hpp"

#include "Utils/Strings.hpp"
#include "Wmi/WmiConnection.hpp"

#include <array>
#include <string_view>

void PhysicalDrivesInformationHandler::run()
{
	using namespace std::literals;
	static constexpr std::array FIELDS = {
		L"Availability"sv,
		L"BytesPerSector"sv,
		L"Capabilities"sv,
		L"CapabilityDescriptions"sv,
		L"Caption"sv,
		L"CompressionMethod"sv,
		L"ConfigManagerErrorCode"sv,
		L"ConfigManagerUserConfig"sv,
		L"CreationClassName"sv,
		L"DefaultBlockSize"sv,
		L"Description"sv,
		L"DeviceID"sv,
		L"ErrorCleared"sv,
		L"ErrorDescription"sv,
		L"ErrorMethodology"sv,
		L"FirmwareRevision"sv,
		L"Index"sv,
		L"InstallDate"sv,
		L"InterfaceType"sv,
		L"LastErrorCode"sv,
		L"Manufacturer"sv,
		L"MaxBlockSize"sv,
		L"MaxMediaSize"sv,
		L"MediaLoaded"sv,
		L"MediaType"sv,
		L"MinBlockSize"sv,
		L"Model"sv,
		L"Name"sv,
		L"NeedsCleaning"sv,
		L"NumberOfMediaSupported"sv,
		L"Partitions"sv,
		L"PNPDeviceID"sv,
		L"PowerManagementCapabilities"sv,
		L"PowerManagementSupported"sv,
		L"SCSIBus"sv,
		L"SCSILogicalUnit"sv,
		L"SCSIPort"sv,
		L"SCSITargetId"sv,
		L"SectorsPerTrack"sv,
		L"SerialNumber"sv,
		L"Signature"sv,
		L"Size"sv,
		L"Status"sv,
		L"StatusInfo"sv,
		L"SystemCreationClassName"sv,
		L"SystemName"sv,
		L"TotalCylinders"sv,
		L"TotalHeads"sv,
		L"TotalSectors"sv,
		L"TotalTracks"sv,
		L"TracksPerCylinder"sv
	};
	const WmiConnection connection;
	const std::vector<std::unique_ptr<WmiResult>> results = connection.query(L"SELECT * FROM Win32_DiskDrive");
	WmiResult& os = *results.front();
	static constexpr std::wstring_view PAIR_SUFFIX = L"\n";
	static constexpr std::wstring_view FIELD_VALUE_SEPARATOR = L": ";
	static constexpr std::wstring_view UNKNOWN_VALUE = L"?";
	for (const std::wstring_view& field : FIELDS)
	{
		const std::optional<std::wstring> result = os.get_formatted_property(std::wstring{field});
		const std::wstring value = result.has_value() ? result.value() : std::wstring{UNKNOWN_VALUE};
		const Buffer data = Strings::to_buffer(
			std::wstring{field} + std::wstring{FIELD_VALUE_SEPARATOR} + value + std::wstring{PAIR_SUFFIX}
		);
		append(data);
	}
	finished();
}
