#include "Commands/LoadFigCommand.hpp"

LoadFigCommand::LoadFigCommand(const Id id, Buffer fig_buffer):
	ICommand(id),
	m_fig_buffer(std::move(fig_buffer))
{
}

ICommand::Type LoadFigCommand::type() const
{
	return Type::LOAD_FIG;
}
