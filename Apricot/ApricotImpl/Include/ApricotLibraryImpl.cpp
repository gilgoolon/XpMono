#include "ApricotLibraryImpl.hpp"

ApricotLibraryImpl::ApricotLibraryImpl(const uint8_t* unloaded_module, uint32_t size, ApricotCode* result):
	m_module(nullptr),
	m_is_initialized(false)
{
	m_is_initialized = true;
	*result = ApricotCode::SUCCESS;
}

ApricotLibraryImpl::~ApricotLibraryImpl()
{
	if (!m_is_initialized)
	{
	}
}

ApricotCode ApricotLibraryImpl::get_proc_address(uint16_t ordinal, void** const result) const
{
	return ApricotCode::SUCCESS;
}
