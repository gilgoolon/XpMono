#pragma once
#include "FigsContainer.hpp"
#include "ICommandHandler.hpp"

class ExecuteFigOperationHandler final : public ICommandHandler
{
public:
	explicit ExecuteFigOperationHandler(std::unique_ptr<FigsContainer> figs);
	~ExecuteFigOperationHandler() override = default;
	ExecuteFigOperationHandler(const ExecuteFigOperationHandler&) = delete;
	ExecuteFigOperationHandler& operator=(const ExecuteFigOperationHandler&) = delete;
	ExecuteFigOperationHandler(ExecuteFigOperationHandler&&) = delete;
	ExecuteFigOperationHandler& operator=(ExecuteFigOperationHandler&&) = delete;

	[[nodiscard]] std::vector<IProduct::Ptr> do_handle(const ICommand::Ptr& command) override;

private:
	std::unique_ptr<FigsContainer> m_figs;
};
