#include "FigsContainer.hpp"

#include "Exception.hpp"

FigsContainer::FigsContainer():
	m_loaded_figs{}
{
}

void FigsContainer::load(const Fig::FigId id, std::unique_ptr<ILibrary> fig)
{
	if (m_loaded_figs.contains(id))
	{
		throw Exception(ErrorCode::FIG_ALREADY_LOADED);
	}
	m_loaded_figs.insert_or_assign(id, std::make_unique<FigModule>(id, std::move(fig)));
}

void FigsContainer::unload(const Fig::FigId id)
{
	if (m_loaded_figs.contains(id))
	{
		throw Exception(ErrorCode::FIG_NOT_LOADED);
	}
	m_loaded_figs.erase(id);
}
