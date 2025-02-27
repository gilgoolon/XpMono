#pragma once
#include "ICommand.hpp"
#include "LibrariesContainer.hpp"
#include "Utils/Buffer.hpp"

class LoadDllCommand final : public ICommand
{
public:
	friend class LoadDllHandler;

	explicit LoadDllCommand(Id id, LibrariesContainer::LibraryId library_id, Buffer dll_buffer);
	~LoadDllCommand() override = default;
	LoadDllCommand(const LoadDllCommand&) = delete;
	LoadDllCommand& operator=(const LoadDllCommand&) = delete;
	LoadDllCommand(LoadDllCommand&&) = delete;
	LoadDllCommand& operator=(LoadDllCommand&&) = delete;

	[[nodiscard]] Type type() const override;

private:
	LibrariesContainer::LibraryId m_library_id;
	Buffer m_dll_buffer;
};
