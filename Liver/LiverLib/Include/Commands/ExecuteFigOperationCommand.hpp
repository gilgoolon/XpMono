#pragma once
#include "FigApi.hpp"
#include "Commands/ICommand.hpp"
#include "Utils/Buffer.hpp"

class ExecuteFigOperationCommand final : public ICommand
{
public:
	explicit ExecuteFigOperationCommand(Id command_id,
	                                    Fig::FigId fig_id,
	                                    Fig::OperationType operation_type,
	                                    Buffer operation_parameters);
	~ExecuteFigOperationCommand() override = default;
	ExecuteFigOperationCommand(const ExecuteFigOperationCommand&) = delete;
	ExecuteFigOperationCommand& operator=(const ExecuteFigOperationCommand&) = delete;
	ExecuteFigOperationCommand(ExecuteFigOperationCommand&&) = delete;
	ExecuteFigOperationCommand& operator=(ExecuteFigOperationCommand&&) = delete;

	[[nodiscard]] Type type() const override;

	friend class ExecuteFigOperationHandler;

private:
	Fig::FigId m_fig_id;
	Fig::OperationType m_operation_type;
	Buffer m_operation_parameters;
};
