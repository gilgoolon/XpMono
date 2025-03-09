#pragma once
#include "ICommandHandler.hpp"
#include "LibrariesContainer.hpp"

class CallDllGenericProcedureHandler final : public ICommandHandler
{
public:
	explicit CallDllGenericProcedureHandler(std::shared_ptr<LibrariesContainer> libraries);
	~CallDllGenericProcedureHandler() override = default;
	CallDllGenericProcedureHandler(const CallDllGenericProcedureHandler&) = delete;
	CallDllGenericProcedureHandler& operator=(const CallDllGenericProcedureHandler&) = delete;
	CallDllGenericProcedureHandler(CallDllGenericProcedureHandler&&) = delete;
	CallDllGenericProcedureHandler& operator=(CallDllGenericProcedureHandler&&) = delete;

	[[nodiscard]] std::vector<IProduct::Ptr> do_handle(const ICommand::Ptr& command) override;

private:
	std::shared_ptr<LibrariesContainer> m_libraries;
};
