#pragma once
#include "IOperationHandler.hpp"

class WmiQuerySingleOperationHandler : public IOperationHandler
{
public:
	explicit WmiQuerySingleOperationHandler(std::unique_ptr<Event> operation_event,
	                                        const std::wstring& class_name,
	                                        std::vector<std::wstring>&& fields);
	~WmiQuerySingleOperationHandler() override = default;
	WmiQuerySingleOperationHandler(const WmiQuerySingleOperationHandler&) = delete;
	WmiQuerySingleOperationHandler& operator=(const WmiQuerySingleOperationHandler&) = delete;
	WmiQuerySingleOperationHandler(WmiQuerySingleOperationHandler&&) = delete;
	WmiQuerySingleOperationHandler& operator=(WmiQuerySingleOperationHandler&&) = delete;

	void run() override;

private:
	std::wstring m_class_name;
	std::vector<std::wstring> m_fields;
};
