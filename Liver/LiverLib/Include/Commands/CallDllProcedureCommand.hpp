#pragma once
#include "Commands/ICommand.hpp"

class CallDllProcedureCommand final : public ICommand
{
public:
	explicit CallDllProcedureCommand(Id command_id, uint32_t library_id, uint16_t ordinal);
	~CallDllProcedureCommand() override = default;
	CallDllProcedureCommand(const CallDllProcedureCommand&) = delete;
	CallDllProcedureCommand& operator=(const CallDllProcedureCommand&) = delete;
	CallDllProcedureCommand(CallDllProcedureCommand&&) = delete;
	CallDllProcedureCommand& operator=(CallDllProcedureCommand&&) = delete;

	[[nodiscard]] Type type() const override;

	friend class CallDllProcedureHandler;

private:
	uint32_t m_library_id;
	uint16_t m_ordinal;
};
