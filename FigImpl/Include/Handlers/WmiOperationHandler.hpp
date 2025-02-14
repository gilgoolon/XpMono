#pragma once
#include "IOperationHandler.hpp"

class WmiOperationHandler : public IOperationHandler
{
public:
	explicit WmiOperationHandler(std::unique_ptr<Event> operation_event,
	                             const std::wstring& class_name,
	                             std::vector<std::wstring>&& fields);
	~WmiOperationHandler() override = default;
	WmiOperationHandler(const WmiOperationHandler&) = delete;
	WmiOperationHandler& operator=(const WmiOperationHandler&) = delete;
	WmiOperationHandler(WmiOperationHandler&&) = delete;
	WmiOperationHandler& operator=(WmiOperationHandler&&) = delete;

	void run() override;

private:
	std::wstring m_class_name;
	std::vector<std::wstring> m_fields;
};
