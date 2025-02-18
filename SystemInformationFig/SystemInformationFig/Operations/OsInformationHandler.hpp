﻿#pragma once
#include "Handlers/IOperationHandler.hpp"
#include "Handlers/WmiQuerySingleOperationHandler.hpp"

class OsInformationHandler final : public WmiQuerySingleOperationHandler
{
public:
	static constexpr Fig::OperationType TYPE = 1;

	explicit OsInformationHandler(std::unique_ptr<Event> operation_event);
	~OsInformationHandler() override = default;
	OsInformationHandler(const OsInformationHandler&) = delete;
	OsInformationHandler& operator=(const OsInformationHandler&) = delete;
	OsInformationHandler(OsInformationHandler&&) = delete;
	OsInformationHandler& operator=(OsInformationHandler&&) = delete;
};
