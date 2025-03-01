#pragma once
#include "ICommandHandler.hpp"
#include "LoadedFigs.hpp"

class LoadFigHandler final : public ICommandHandler
{
public:
	explicit LoadFigHandler(std::shared_ptr<LoadedFigs> figs);
	~LoadFigHandler() override = default;
	LoadFigHandler(const LoadFigHandler&) = delete;
	LoadFigHandler& operator=(const LoadFigHandler&) = delete;
	LoadFigHandler(LoadFigHandler&&) = delete;
	LoadFigHandler& operator=(LoadFigHandler&&) = delete;

	[[nodiscard]] std::vector<IProduct::Ptr> do_handle(const ICommand::Ptr& command) override;

private:
	std::shared_ptr<LoadedFigs> m_figs;
};
