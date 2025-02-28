#pragma once
#include "ICommandHandler.hpp"
#include "Commands/LoadDllCommand.hpp"

class LoadDllHandler final : public ICommandHandler
{
public:
	explicit LoadDllHandler(std::shared_ptr<LibrariesContainer> libraries);
	~LoadDllHandler() override = default;
	LoadDllHandler(const LoadDllHandler&) = delete;
	LoadDllHandler& operator=(const LoadDllHandler&) = delete;
	LoadDllHandler(LoadDllHandler&&) = delete;
	LoadDllHandler& operator=(LoadDllHandler&&) = delete;

	[[nodiscard]] std::vector<IProduct::Ptr> do_handle(ICommand::Ptr command) override;

private:
	std::shared_ptr<LibrariesContainer> m_libraries;
};
