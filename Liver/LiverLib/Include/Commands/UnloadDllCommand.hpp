#pragma once
#include "ICommand.hpp"
#include "LibrariesContainer.hpp"

class UnloadDllCommand final : public ICommand
{
public:
	explicit UnloadDllCommand(Id id, LibrariesContainer::LibraryId library_id);
	~UnloadDllCommand() override = default;
	UnloadDllCommand(const UnloadDllCommand&) = delete;
	UnloadDllCommand& operator=(const UnloadDllCommand&) = delete;
	UnloadDllCommand(UnloadDllCommand&&) = delete;
	UnloadDllCommand& operator=(UnloadDllCommand&&) = delete;

	[[nodiscard]] Type type() const override;

private:
	LibrariesContainer::LibraryId m_library_id;
};
