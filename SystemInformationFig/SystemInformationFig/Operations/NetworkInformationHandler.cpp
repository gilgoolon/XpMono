#include "NetworkInformationHandler.hpp"

NetworkInformationHandler::NetworkInformationHandler(std::unique_ptr<Event> operation_event) :
	WmiQueryMultiOperationHandler(
		std::move(operation_event),
		L"Win32_NetworkAdapterConfiguration",
		{
			L"DefaultIPGateway",
			L"DHCPServer",
			L"IPAddress",
			L"IPSubnet",
			L"MACAddress",
			L"DNSServerSearchOrder",
			L"Description"
		}
	)
{
}
