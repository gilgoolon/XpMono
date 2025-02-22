#pragma once
#include "ICommandHandler.hpp"
#include "Commands/LoadDllCommand.hpp"

class LoadDllHandler final : public ICommandHandler
{
public:
	explicit LoadDllHandler(ICommand::Ptr command);
	~LoadDllHandler() override = default;
	LoadDllHandler(const LoadDllHandler&) = delete;
	LoadDllHandler& operator=(const LoadDllHandler&) = delete;
	LoadDllHandler(LoadDllHandler&&) = delete;
	LoadDllHandler& operator=(LoadDllHandler&&) = delete;

	[[nodiscard]] std::vector<IProduct::Ptr> handle(LiverContext context) const override;

private:
	std::shared_ptr<LoadDllCommand> m_command;
};
