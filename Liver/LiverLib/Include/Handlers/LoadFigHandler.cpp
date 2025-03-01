#include "LoadFigHandler.hpp"

#include "Commands/LoadFigCommand.hpp"

LoadFigHandler::LoadFigHandler(std::shared_ptr<LoadedFigs> figs):
	m_figs(std::move(figs))
{
}

std::vector<IProduct::Ptr> LoadFigHandler::do_handle(const ICommand::Ptr& command)
{
	const auto load_fig_command = std::dynamic_pointer_cast<LoadFigCommand>(command);
	m_figs->load(load_fig_command->m_fig_id, load_fig_command->m_fig_buffer);
	return {};
}
