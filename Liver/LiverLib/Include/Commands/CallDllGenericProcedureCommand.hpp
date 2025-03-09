#pragma once
#include "ICommand.hpp"
#include "Utils/Buffer.hpp"

class CallDllGenericProcedureCommand final : public ICommand
{
public:
	explicit CallDllGenericProcedureCommand(Id command_id, uint32_t library_id, uint16_t ordinal, Buffer parameters);
	~CallDllGenericProcedureCommand() override = default;
	CallDllGenericProcedureCommand(const CallDllGenericProcedureCommand&) = delete;
	CallDllGenericProcedureCommand& operator=(const CallDllGenericProcedureCommand&) = delete;
	CallDllGenericProcedureCommand(CallDllGenericProcedureCommand&&) = delete;
	CallDllGenericProcedureCommand& operator=(CallDllGenericProcedureCommand&&) = delete;

	[[nodiscard]] Type type() const override;

	friend class CallDllGenericProcedureHandler;

private:
	uint32_t m_library_id;
	uint16_t m_ordinal;
	Buffer m_parameters;
};
