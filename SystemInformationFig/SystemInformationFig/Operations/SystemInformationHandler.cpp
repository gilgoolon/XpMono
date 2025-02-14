#include "SystemInformationHandler.hpp"

#include "Trace.hpp"
#include "Utils/Strings.hpp"
#include "Wmi/WmiConnection.hpp"
#include <array>
#include <string_view>

SystemInformationHandler::SystemInformationHandler(std::unique_ptr<Event> operation_event):
	IOperationHandler(std::move(operation_event))
{
}

void SystemInformationHandler::run()
{
	using namespace std::string_view_literals;
	static constexpr std::array FIELDS = {
		L"BootDevice"sv,
		L"BuildNumber"sv,
		L"BuildType"sv,
		L"Caption"sv,
		L"CodeSet"sv,
		L"CountryCode"sv,
		L"CreationClassName"sv,
		L"CSCreationClassName"sv,
		L"CSDVersion"sv,
		L"CSName"sv,
		L"CurrentTimeZone"sv,
		L"DataExecutionPrevention_Available"sv,
		L"DataExecutionPrevention_32BitApplications"sv,
		L"DataExecutionPrevention_Drivers"sv,
		L"DataExecutionPrevention_SupportPolicy"sv,
		L"Debug"sv,
		L"Description"sv,
		L"Distributed"sv,
		L"EncryptionLevel"sv,
		L"ForegroundApplicationBoost"sv,
		L"FreePhysicalMemory"sv,
		L"FreeSpaceInPagingFiles"sv,
		L"FreeVirtualMemory"sv,
		L"InstallDate"sv,
		L"LargeSystemCache"sv,
		L"LastBootUpTime"sv,
		L"LocalDateTime"sv,
		L"Locale"sv,
		L"Manufacturer"sv,
		L"MaxNumberOfProcesses"sv,
		L"MaxProcessMemorySize"sv,
		L"MUILanguages"sv,
		L"Name"sv,
		L"NumberOfLicensedUsers"sv,
		L"NumberOfProcesses"sv,
		L"NumberOfUsers"sv,
		L"OperatingSystemSKU"sv,
		L"Organization"sv,
		L"OSArchitecture"sv,
		L"OSLanguage"sv,
		L"OSProductSuite"sv,
		L"OSType"sv,
		L"OtherTypeDescription"sv,
		L"PAEEnabled"sv,
		L"PlusProductID"sv,
		L"PlusVersionNumber"sv,
		L"PortableOperatingSystem"sv,
		L"Primary"sv,
		L"ProductType"sv,
		L"RegisteredUser"sv,
		L"SerialNumber"sv,
		L"ServicePackMajorVersion"sv,
		L"ServicePackMinorVersion"sv,
		L"SizeStoredInPagingFiles"sv,
		L"Status"sv,
		L"SuiteMask"sv,
		L"SystemDevice"sv,
		L"SystemDirectory"sv,
		L"SystemDrive"sv,
		L"TotalSwapSpaceSize"sv,
		L"TotalVirtualMemorySize"sv,
		L"TotalVisibleMemorySize"sv,
		L"Version"sv,
		L"WindowsDirectory"sv,
		L"QuantumLength"sv,
		L"QuantumType"sv,
	};
	const WmiConnection connection;
	const std::vector<std::unique_ptr<WmiResult>> results = connection.query(L"SELECT * FROM Win32_OperatingSystem");
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
