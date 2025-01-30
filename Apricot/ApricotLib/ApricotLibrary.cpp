#include "ApricotLibrary.hpp"

#include "ApricotCode.hpp"
#include "ApricotException.hpp"
#include "Trace.hpp"

ApricotLibrary::ApricotLibrary(const Buffer& buffer):
	m_library(nullptr)
{
	auto result = ApricotCode::UNKNOWN_ERROR;
	m_library = std::make_unique<ApricotLibraryImpl>(buffer.data(), buffer.size(), &result);
	if (result != ApricotCode::SUCCESS)
	{
		throw ApricotException(ErrorCode::FAILED_APRICOT_LOAD, result);
	}
}

void* ApricotLibrary::get_exported_procedure(const uint16_t ordinal) const
{
	void* result = nullptr;
	const ApricotCode code = m_library->get_proc_address(ordinal, &result);
	if (code != ApricotCode::SUCCESS)
	{
		throw ApricotException(ErrorCode::FAILED_APRICOT_GET, code);
	}
	return result;
}
