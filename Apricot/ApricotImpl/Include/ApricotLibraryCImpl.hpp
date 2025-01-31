#pragma once
#include "ApricotCode.hpp"

class ApricotLibraryImpl;

struct ApricotLibraryContext final
{
	ApricotLibraryImpl* impl;
};

ApricotCode APRICOT_LIBRARY__create(__out ApricotLibraryContext* ctx, const uint8_t* buffer, uint32_t size);

ApricotCode APRICOT_LIBRARY__get_proc_address(const ApricotLibraryContext* ctx, uint16_t ordinal, __out void*& result);

ApricotCode APRICOT_LIBRARY__get_proc_address(const ApricotLibraryContext* ctx,
                                              const char* name,
                                              __out void*& result);

void APRICOT_LIBRARY__destroy(ApricotLibraryContext* ctx);
