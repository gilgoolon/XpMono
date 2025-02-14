#pragma once
#include "Handlers/WmiQueryMultiOperationHandler.hpp"

class UsersInformationHandler final : public WmiQueryMultiOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = 4;

	explicit UsersInformationHandler(std::unique_ptr<Event> operation_event);
	~UsersInformationHandler() override = default;
	UsersInformationHandler(const UsersInformationHandler&) = delete;
	UsersInformationHandler& operator=(const UsersInformationHandler&) = delete;
	UsersInformationHandler(UsersInformationHandler&&) = delete;
	UsersInformationHandler& operator=(UsersInformationHandler&&) = delete;
};
