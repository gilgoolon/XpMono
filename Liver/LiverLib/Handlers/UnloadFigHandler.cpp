#include "Handlers/UnloadFigHandler.hpp"

#include "Commands/UnloadFigCommand.hpp"

UnloadFigHandler::UnloadFigHandler(std::shared_ptr<FigsContainer> figs):
	m_figs(std::move(figs))
{
}

std::vector<IProduct::Ptr> UnloadFigHandler::do_handle(const ICommand::Ptr& command)
{
	const auto unload_fig_command = std::dynamic_pointer_cast<UnloadFigCommand>(command);
	m_figs->unload(unload_fig_command->m_fig_id);
	return {};
}
