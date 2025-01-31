#include "ApricotLibrary.hpp"

#include "ApricotCode.hpp"
#include "ApricotException.hpp"
#include "ApricotLibraryCImpl.hpp"
#include "Trace.hpp"

ApricotLibrary::ApricotLibrary(const Buffer& buffer):
	m_library(std::make_unique<ApricotLibraryContext>())
{
	const ApricotCode result = APRICOT_LIBRARY__create(m_library.get(), buffer.data(), buffer.size());
	if (result != ApricotCode::SUCCESS)
	{
		throw ApricotException(ErrorCode::FAILED_APRICOT_LOAD, result);
	}
}

ApricotLibrary::~ApricotLibrary()
{
	try
	{
		APRICOT_LIBRARY__destroy(m_library.get());
	}
	catch (...)
	{
		TRACE(L"failed to unload Apricot library")
	}
}

void* ApricotLibrary::get_exported_procedure(const uint16_t ordinal) const
{
	void* result = nullptr;
	const ApricotCode code = APRICOT_LIBRARY__get_proc_address(m_library.get(), ordinal, result);
	if (code != ApricotCode::SUCCESS)
	{
		throw ApricotException(ErrorCode::FAILED_APRICOT_GET, code);
	}
	return result;
}

void* ApricotLibrary::get_exported_procedure(const std::string& name) const
{
	void* result = nullptr;
	const ApricotCode code = APRICOT_LIBRARY__get_proc_address(m_library.get(), name.c_str(), result);
	if (code != ApricotCode::SUCCESS)
	{
		throw ApricotException(ErrorCode::FAILED_APRICOT_GET, code);
	}
	return result;
}
