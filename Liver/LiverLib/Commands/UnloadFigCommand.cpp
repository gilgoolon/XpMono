#include "Commands/UnloadFigCommand.hpp"

UnloadFigCommand::UnloadFigCommand(const Id id, const Fig::FigId fig_id) :
	ICommand(id),
	m_fig_id(fig_id)
{
}

ICommand::Type UnloadFigCommand::type() const
{
	return Type::UNLOAD_FIG;
}
