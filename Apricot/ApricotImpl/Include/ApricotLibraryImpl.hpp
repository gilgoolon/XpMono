#pragma once
#include "ApricotCode.hpp"

#include <cstdint>

class ApricotLibraryImpl final
{
public:
	explicit ApricotLibraryImpl(const uint8_t* unloaded_module, uint32_t size, __out ApricotCode* result);
	~ApricotLibraryImpl();
	ApricotLibraryImpl(const ApricotLibraryImpl&) = delete;
	ApricotLibraryImpl& operator=(const ApricotLibraryImpl&) = delete;
	ApricotLibraryImpl(ApricotLibraryImpl&&) = delete;
	ApricotLibraryImpl& operator=(ApricotLibraryImpl&&) = delete;

	[[nodiscard]] ApricotCode get_proc_address(uint16_t ordinal, __out void** result) const;

private:
	void* m_module;

	bool m_is_initialized;
};
