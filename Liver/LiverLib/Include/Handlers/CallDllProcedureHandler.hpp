#pragma once
#include "ICommandHandler.hpp"
#include "LibrariesContainer.hpp"

class CallDllProcedureHandler final : public ICommandHandler
{
public:
	explicit CallDllProcedureHandler(std::shared_ptr<LibrariesContainer> libraries);
	~CallDllProcedureHandler() override = default;
	CallDllProcedureHandler(const CallDllProcedureHandler&) = delete;
	CallDllProcedureHandler& operator=(const CallDllProcedureHandler&) = delete;
	CallDllProcedureHandler(CallDllProcedureHandler&&) = delete;
	CallDllProcedureHandler& operator=(CallDllProcedureHandler&&) = delete;

	[[nodiscard]] std::vector<IProduct::Ptr> do_handle(const ICommand::Ptr& command) override;

private:
	std::shared_ptr<LibrariesContainer> m_libraries;
};
