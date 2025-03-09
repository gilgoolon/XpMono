#pragma once
#include "Synchronization/CriticalSection.hpp"
#include "Utils/Buffer.hpp"

#include <ApricotLibrary.hpp>
#include <cstdint>
#include <unordered_map>

class LibrariesContainer final
{
public:
	using LibraryId = uint32_t;

	explicit LibrariesContainer() = default;
	~LibrariesContainer() = default;
	LibrariesContainer(const LibrariesContainer&) = delete;
	LibrariesContainer& operator=(const LibrariesContainer&) = delete;
	LibrariesContainer(LibrariesContainer&&) = delete;
	LibrariesContainer& operator=(LibrariesContainer&&) = delete;

	void load(LibraryId library_id, const Buffer& library_buffer);
	void unload(LibraryId library_id);

	using GenericFunctionProto = uint32_t (__cdecl *)(const uint8_t*, uint32_t, uint8_t*, uint32_t*);
	static constexpr uint32_t GENERIC_FUNCTION_SUCCESS = FALSE;

	[[nodiscard]] Buffer call(LibraryId library_id, uint16_t ordinal, const Buffer& arguments);
	void call(LibraryId library_id, uint16_t ordinal);

private:
	CriticalSection m_loader_lock;
	std::unordered_map<LibraryId, std::unique_ptr<ApricotLibrary>> m_libraries;
};
