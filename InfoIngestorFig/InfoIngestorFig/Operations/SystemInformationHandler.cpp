#include "SystemInformationHandler.hpp"

#include "Trace.hpp"
#include "Utils/Strings.hpp"
#include "Wmi/WmiConnection.hpp"
#include <array>
#include <string_view>

SystemInformationHandler::SystemInformationHandler(std::unique_ptr<Event> operation_event):
	m_operation_event(std::move(operation_event)),
	m_status(Fig::ExecutionStatus::EXECUTING),
	m_code(Fig::FIG_SPECIFIC_CODE_RESERVED),
	m_result_left{},
	m_result_lock()
{
}

Buffer SystemInformationHandler::take([[maybe_unused]] const uint32_t max_size)
{
	const CriticalSection::Acquired acquired = m_result_lock.acquire();
	const uint32_t taken_size = min(max_size, m_result_left.size());
	Buffer result = {m_result_left.begin(), m_result_left.begin() + static_cast<int32_t>(taken_size)};
	if (result.empty() && m_status == Fig::ExecutionStatus::EXECUTING_CAN_TAKE)
	{
		m_status = Fig::ExecutionStatus::EXECUTING;
	}
	m_result_left.erase(m_result_left.begin(), m_result_left.begin() + static_cast<int32_t>(taken_size));
	return result;
}

Fig::ExecutionStatus SystemInformationHandler::status()
{
	return m_status;
}

Fig::FigSpecificCode SystemInformationHandler::specific_code()
{
	return m_code;
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
	for (const std::wstring_view& field : FIELDS)
	{
		const std::wstring value = os.get_property(std::wstring{field});
		const Buffer data = Strings::to_buffer(
			std::wstring{field} + std::wstring{FIELD_VALUE_SEPARATOR} + value + std::wstring{PAIR_SUFFIX}
		);
		CriticalSection::Acquired acquired = m_result_lock.acquire();
		m_result_left.insert(m_result_left.end(), data.begin(), data.end());
		m_status = Fig::ExecutionStatus::EXECUTING_CAN_TAKE;
		m_operation_event->set();
	}
	CriticalSection::Acquired acquired = m_result_lock.acquire();
	m_status = Fig::ExecutionStatus::FINISHED;
	m_operation_event->set();
}
