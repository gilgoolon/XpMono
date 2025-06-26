#include "IatHook.hpp"

IatHook::IatHook(const Pe::ImportedFunctionsIterator::ImportedFunction& function, void* hook):
	m_function_pointer(reinterpret_cast<void**>(function.bound->u1.Function)),
	m_original_value(*m_function_pointer),
	m_is_initialized(false)
{
	*m_function_pointer = hook;
	m_is_initialized = true;
}

IatHook::~IatHook()
{
	if (!m_is_initialized)
	{
		*m_function_pointer = m_original_value;
	}
}
