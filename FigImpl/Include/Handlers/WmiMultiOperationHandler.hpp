#pragma once
#include "IOperationHandler.hpp"

class WmiMultiOperationHandler final : public IOperationHandler
{
public:
	explicit WmiMultiOperationHandler(std::unique_ptr<Event> operation_event,
	                                  const std::wstring& class_name,
	                                  std::vector<std::wstring>&& fields);
	~WmiMultiOperationHandler() override = default;
	WmiMultiOperationHandler(const WmiMultiOperationHandler&) = delete;
	WmiMultiOperationHandler& operator=(const WmiMultiOperationHandler&) = delete;
	WmiMultiOperationHandler(WmiMultiOperationHandler&&) = delete;
	WmiMultiOperationHandler& operator=(WmiMultiOperationHandler&&) = delete;

	void run() override;

private:
	std::wstring m_class_name;
	std::vector<std::wstring> m_fields;
};
