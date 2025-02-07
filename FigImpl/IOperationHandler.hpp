#pragma once
#include "FigApi.hpp"
#include "Utils/Buffer.hpp"

class IOperationHandler
{
public:
	explicit IOperationHandler();
	virtual ~IOperationHandler() = default;
	IOperationHandler(const IOperationHandler&) = delete;
	IOperationHandler& operator=(const IOperationHandler&) = delete;
	IOperationHandler(IOperationHandler&&) = delete;
	IOperationHandler& operator=(IOperationHandler&&) = delete;

	[[nodiscard]] virtual Buffer take(uint32_t max_size) = 0;
	[[nodiscard]] virtual Fig::ExecutionStatus status();
	[[nodiscard]] virtual Fig::FigSpecificCode specific_code();

	void error(Fig::FigSpecificCode specific_code);

	virtual void run() = 0;

protected:
	Fig::ExecutionStatus m_status;
	Fig::FigSpecificCode m_specific_code;
};
