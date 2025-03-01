#include "FigsContainer.hpp"

#include "Exception.hpp"

FigsContainer::FigsContainer():
	m_loaded_figs{}
{
}

void FigsContainer::load(const Fig::FigId id, const Buffer& buffer)
{
	if (m_loaded_figs.contains(id))
	{
		throw Exception(ErrorCode::FIG_ALREADY_LOADED);
	}
	m_loaded_figs.insert_or_assign(id, std::make_unique<FigModule>(id, buffer));
}

void FigsContainer::unload(const Fig::FigId id)
{
	if (m_loaded_figs.contains(id))
	{
		throw Exception(ErrorCode::FIG_NOT_LOADED);
	}
	m_loaded_figs.erase(id);
}
