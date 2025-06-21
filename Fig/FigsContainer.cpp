#include "FigsContainer.hpp"

#include "Exception.hpp"

FigsContainer::FigsContainer():
	m_lock(),
	m_loaded_figs{}
{
}

void FigsContainer::load(std::unique_ptr<ILibrary> fig)
{
	auto acquired = m_lock.acquire();

	auto fig_module = std::make_unique<FigModule>(std::move(fig));
	if (m_loaded_figs.contains(fig_module->id()))
	{
		throw Exception(ErrorCode::FIG_ALREADY_LOADED);
	}

	m_loaded_figs.insert_or_assign(fig_module->id(), std::move(fig_module));
}

void FigsContainer::unload(const Fig::FigId id)
{
	auto acquired = m_lock.acquire();

	if (!m_loaded_figs.contains(id))
	{
		throw Exception(ErrorCode::FIG_NOT_LOADED);
	}
	m_loaded_figs.erase(id);
}

std::unique_ptr<FigOperation> FigsContainer::execute(const Fig::FigId id,
                                                     const Fig::OperationType operation_type,
                                                     const Buffer& operation_parameters)
{
	auto acquired = m_lock.acquire();
	if (!m_loaded_figs.contains(id))
	{
		throw Exception(ErrorCode::FIG_NOT_LOADED);
	}

	return m_loaded_figs[id]->execute(operation_type, operation_parameters);
}
