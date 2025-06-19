#pragma once
#include "Handlers/WmiQueryMultiOperationHandler.hpp"
#include "../Api.hpp"

class UsersInformationHandler final : public WmiQueryMultiOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = static_cast<Fig::OperationType>(Api::OperationType::USERS_INFO);

	explicit UsersInformationHandler(std::unique_ptr<Event> operation_event);
	~UsersInformationHandler() override = default;
	UsersInformationHandler(const UsersInformationHandler&) = delete;
	UsersInformationHandler& operator=(const UsersInformationHandler&) = delete;
	UsersInformationHandler(UsersInformationHandler&&) = delete;
	UsersInformationHandler& operator=(UsersInformationHandler&&) = delete;
};
