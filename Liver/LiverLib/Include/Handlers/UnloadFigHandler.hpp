#pragma once
#include "FigsContainer.hpp"
#include "ICommandHandler.hpp"

class UnloadFigHandler final : public ICommandHandler
{
public:
	explicit UnloadFigHandler(std::shared_ptr<FigsContainer> figs);
	~UnloadFigHandler() override = default;
	UnloadFigHandler(const UnloadFigHandler&) = delete;
	UnloadFigHandler& operator=(const UnloadFigHandler&) = delete;
	UnloadFigHandler(UnloadFigHandler&&) = delete;
	UnloadFigHandler& operator=(UnloadFigHandler&&) = delete;

	[[nodiscard]] std::vector<IProduct::Ptr> do_handle(const ICommand::Ptr& command) override;

private:
	std::shared_ptr<FigsContainer> m_figs;
};
