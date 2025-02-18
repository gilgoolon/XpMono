﻿#include "ApricotLibraryCImpl.hpp"

#include "ApricotLibraryImpl.hpp"
#include "Std.hpp"

ApricotCode APRICOT_LIBRARY__create(ApricotLibraryContext* ctx, const uint8_t* buffer, const uint32_t size)
{
	auto result = ApricotCode::UNKNOWN_ERROR;
	ctx->impl = new ApricotLibraryImpl(buffer, size, result);
	return result;
}

ApricotCode APRICOT_LIBRARY__get_proc_address(const ApricotLibraryContext* ctx,
                                              const uint16_t ordinal,
                                              void*& result)
{
	return ctx->impl->get_proc_address(ordinal, result);
}

ApricotCode APRICOT_LIBRARY__get_proc_address(const ApricotLibraryContext* ctx, const char* name, void*& result)
{
	return ctx->impl->get_proc_address(name, result);
}

void APRICOT_LIBRARY__destroy(ApricotLibraryContext* const ctx)
{
	delete ctx->impl;
}
