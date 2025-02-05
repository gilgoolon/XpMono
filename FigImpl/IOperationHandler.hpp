#pragma once
#include "FigApi.hpp"
#include "Utils/Buffer.hpp"

class IOperationHandler
{
public:
	explicit IOperationHandler() = default;
	virtual ~IOperationHandler() = default;
	IOperationHandler(const IOperationHandler&) = delete;
	IOperationHandler& operator=(const IOperationHandler&) = delete;
	IOperationHandler(IOperationHandler&&) = delete;
	IOperationHandler& operator=(IOperationHandler&&) = delete;

	[[nodiscard]] virtual Buffer take(uint32_t max_size) = 0;
	[[nodiscard]] virtual Fig::ExecutionStatus status() = 0;
	[[nodiscard]] virtual Fig::FigSpecificCode specific_code() = 0;

	virtual void run() = 0;
};
