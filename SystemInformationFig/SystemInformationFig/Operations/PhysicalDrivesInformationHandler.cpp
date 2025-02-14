#include "PhysicalDrivesInformationHandler.hpp"

#include "Utils/Strings.hpp"
#include "Wmi/WmiConnection.hpp"

PhysicalDrivesInformationHandler::PhysicalDrivesInformationHandler(std::unique_ptr<Event> operation_event):
	WmiQuerySingleOperationHandler(
		std::move(operation_event),
		L"Win32_DiskDrive",
		{
			L"Availability",
			L"BytesPerSector",
			L"Capabilities",
			L"CapabilityDescriptions",
			L"Caption",
			L"CompressionMethod",
			L"ConfigManagerErrorCode",
			L"ConfigManagerUserConfig",
			L"CreationClassName",
			L"DefaultBlockSize",
			L"Description",
			L"DeviceID",
			L"ErrorCleared",
			L"ErrorDescription",
			L"ErrorMethodology",
			L"FirmwareRevision",
			L"Index",
			L"InstallDate",
			L"InterfaceType",
			L"LastErrorCode",
			L"Manufacturer",
			L"MaxBlockSize",
			L"MaxMediaSize",
			L"MediaLoaded",
			L"MediaType",
			L"MinBlockSize",
			L"Model",
			L"Name",
			L"NeedsCleaning",
			L"NumberOfMediaSupported",
			L"Partitions",
			L"PNPDeviceID",
			L"PowerManagementCapabilities",
			L"PowerManagementSupported",
			L"SCSIBus",
			L"SCSILogicalUnit",
			L"SCSIPort",
			L"SCSITargetId",
			L"SectorsPerTrack",
			L"SerialNumber",
			L"Signature",
			L"Size",
			L"Status",
			L"StatusInfo",
			L"SystemCreationClassName",
			L"SystemName",
			L"TotalCylinders",
			L"TotalHeads",
			L"TotalSectors",
			L"TotalTracks",
			L"TracksPerCylinder"
		}
	)
{
}
