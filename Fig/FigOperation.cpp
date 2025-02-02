#include "FigOperation.hpp"

FigOperation::FigOperation(std::shared_ptr<FigModule> module, const Fig::OperationId id, const HANDLE unmanaged_event):
	m_module(std::move(module)),
	m_id(id),
	m_event(unmanaged_event)
{
}

Buffer FigOperation::take()
{
	// TODO
	return {};
}
