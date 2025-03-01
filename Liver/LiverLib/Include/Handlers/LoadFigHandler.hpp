#pragma once
#include "FigsContainer.hpp"
#include "ICommandHandler.hpp"

class LoadFigHandler final : public ICommandHandler
{
public:
	explicit LoadFigHandler(std::shared_ptr<FigsContainer> figs);
	~LoadFigHandler() override = default;
	LoadFigHandler(const LoadFigHandler&) = delete;
	LoadFigHandler& operator=(const LoadFigHandler&) = delete;
	LoadFigHandler(LoadFigHandler&&) = delete;
	LoadFigHandler& operator=(LoadFigHandler&&) = delete;

	[[nodiscard]] std::vector<IProduct::Ptr> do_handle(const ICommand::Ptr& command) override;

private:
	std::shared_ptr<FigsContainer> m_figs;
};
