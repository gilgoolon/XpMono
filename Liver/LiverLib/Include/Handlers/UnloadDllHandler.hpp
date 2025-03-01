#pragma once
#include "ICommandHandler.hpp"
#include "LibrariesContainer.hpp"

class UnloadDllHandler final : public ICommandHandler
{
public:
	explicit UnloadDllHandler(std::shared_ptr<LibrariesContainer> libraries);
	~UnloadDllHandler() override = default;
	UnloadDllHandler(const UnloadDllHandler&) = delete;
	UnloadDllHandler& operator=(const UnloadDllHandler&) = delete;
	UnloadDllHandler(UnloadDllHandler&&) = delete;
	UnloadDllHandler& operator=(UnloadDllHandler&&) = delete;

	std::vector<IProduct::Ptr> do_handle(const ICommand::Ptr& command) override;

private:
	std::shared_ptr<LibrariesContainer> m_libraries;
};
