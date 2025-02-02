#include "LoadedFigs.hpp"

#include "Exception.hpp"

LoadedFigs::LoadedFigs():
	m_loaded_figs{}
{
}

void LoadedFigs::load(const Fig::FigId id, const Buffer& buffer)
{
	if (m_loaded_figs.contains(id))
	{
		throw Exception(ErrorCode::FIG_ALREADY_LOADED);
	}
	m_loaded_figs.insert_or_assign(id, std::make_unique<FigModule>(id, buffer));
}

void LoadedFigs::unload(const Fig::FigId id)
{
	if (m_loaded_figs.contains(id))
	{
		throw Exception(ErrorCode::FIG_NOT_LOADED);
	}
	m_loaded_figs.erase(std::remove(m_loaded_figs.begin(), m_loaded_figs.end(), id), m_loaded_figs.end());
}
