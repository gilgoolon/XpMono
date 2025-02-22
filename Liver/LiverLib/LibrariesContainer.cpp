#include "LibrariesContainer.hpp"

#include "Exception.hpp"

void LibrariesContainer::load(const LibraryId library_id, const Buffer& library_buffer)
{
	CriticalSection::Acquired acquired = m_loader_lock.acquire();

	auto library = std::make_unique<ApricotLibrary>(library_buffer);
	m_libraries.insert_or_assign(library_id, std::move(library));
}

void LibrariesContainer::unload(const LibraryId library_id)
{
	CriticalSection::Acquired acquired = m_loader_lock.acquire();

	m_libraries.erase(library_id);
}

Buffer LibrariesContainer::call(const LibraryId library_id, const uint16_t ordinal, const Buffer& arguments)
{
	CriticalSection::Acquired acquired = m_loader_lock.acquire();

	static constexpr uint32_t DEFAULT_OUTPUT_BUFFER_SIZE = 4096;
	uint32_t output_size = DEFAULT_OUTPUT_BUFFER_SIZE;
	Buffer output(output_size);

	const uint32_t result = m_libraries.at(library_id)->call<GenericFunctionProto>(
		ordinal,
		arguments.data(),
		arguments.size(),
		output.data(),
		&output_size
	);
	if (result != GENERIC_FUNCTION_SUCCESS)
	{
		throw GenericFunctionException(result);
	}
	output.resize(output_size);
	return output;
}
