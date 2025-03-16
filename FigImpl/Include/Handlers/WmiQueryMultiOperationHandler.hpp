#pragma once
#include "IOperationHandler.hpp"

class WmiQueryMultiOperationHandler : public IOperationHandler
{
public:
	explicit WmiQueryMultiOperationHandler(std::unique_ptr<Event> operation_event,
	                                       std::wstring class_name,
	                                       std::vector<std::wstring> fields);
	~WmiQueryMultiOperationHandler() override = default;
	WmiQueryMultiOperationHandler(const WmiQueryMultiOperationHandler&) = delete;
	WmiQueryMultiOperationHandler& operator=(const WmiQueryMultiOperationHandler&) = delete;
	WmiQueryMultiOperationHandler(WmiQueryMultiOperationHandler&&) = delete;
	WmiQueryMultiOperationHandler& operator=(WmiQueryMultiOperationHandler&&) = delete;

	void run() override;

private:
	std::wstring m_class_name;
	std::vector<std::wstring> m_fields;
};
