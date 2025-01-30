#include "ApricotLibraryImpl.hpp"

#include <Windows.h>

ApricotLibraryImpl::ApricotLibraryImpl(const uint8_t* unloaded_module, const uint32_t size, ApricotCode* const result):
	m_module(nullptr),
	m_is_initialized(false)
{
	m_is_initialized = true;
	UNREFERENCED_PARAMETER(unloaded_module);
	UNREFERENCED_PARAMETER(size);
	*result = ApricotCode::SUCCESS;
}

ApricotLibraryImpl::~ApricotLibraryImpl()
{
	if (!m_is_initialized)
	{
	}
}

ApricotCode ApricotLibraryImpl::get_proc_address(const uint16_t ordinal, void** const result) const
{
	UNREFERENCED_PARAMETER(ordinal);
	UNREFERENCED_PARAMETER(result);
	return ApricotCode::SUCCESS;
}
