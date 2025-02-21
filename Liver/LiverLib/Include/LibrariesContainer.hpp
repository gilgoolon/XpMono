#pragma once
#include <cstdint>

class LibrariesContainer final
{
public:
	using LibraryId = uint32_t;

	explicit LibrariesContainer();
	~LibrariesContainer();
	LibrariesContainer(const LibrariesContainer&) = delete;
	LibrariesContainer& operator=(const LibrariesContainer&) = delete;
	LibrariesContainer(LibrariesContainer&&) = delete;
	LibrariesContainer& operator=(LibrariesContainer&&) = delete;
};
