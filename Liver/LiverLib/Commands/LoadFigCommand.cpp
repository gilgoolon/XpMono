#include "Commands/LoadFigCommand.hpp"

LoadFigCommand::LoadFigCommand(const Id id, const Fig::FigId fig_id, Buffer fig_buffer):
	ICommand(id),
	m_fig_id(fig_id),
	m_fig_buffer(std::move(fig_buffer))
{
}

ICommand::Type LoadFigCommand::type() const
{
	return Type::LOAD_FIG;
}
