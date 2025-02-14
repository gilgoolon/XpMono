#pragma once
#include "IOperationHandler.hpp"

class WmiSingleOperationHandler : public IOperationHandler
{
public:
	explicit WmiSingleOperationHandler(std::unique_ptr<Event> operation_event,
	                                   const std::wstring& class_name,
	                                   std::vector<std::wstring>&& fields);
	~WmiSingleOperationHandler() override = default;
	WmiSingleOperationHandler(const WmiSingleOperationHandler&) = delete;
	WmiSingleOperationHandler& operator=(const WmiSingleOperationHandler&) = delete;
	WmiSingleOperationHandler(WmiSingleOperationHandler&&) = delete;
	WmiSingleOperationHandler& operator=(WmiSingleOperationHandler&&) = delete;

	void run() override;

private:
	std::wstring m_class_name;
	std::vector<std::wstring> m_fields;
};
