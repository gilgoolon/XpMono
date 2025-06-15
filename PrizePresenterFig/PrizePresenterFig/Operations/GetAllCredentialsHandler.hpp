#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"

class GetAllCredentialsHandler final : public IOperationHandler
{
public:
	explicit GetAllCredentialsHandler(std::unique_ptr<Event> operation_event);
	~GetAllCredentialsHandler() override = default;
	GetAllCredentialsHandler(const GetAllCredentialsHandler&) = delete;
	GetAllCredentialsHandler& operator=(const GetAllCredentialsHandler&) = delete;
	GetAllCredentialsHandler(GetAllCredentialsHandler&&) = delete;
	GetAllCredentialsHandler& operator=(GetAllCredentialsHandler&&) = delete;

	void run() override;
};
