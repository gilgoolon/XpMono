#include "Commands/UnloadDllCommand.hpp"

UnloadDllCommand::UnloadDllCommand(const Id id, const LibrariesContainer::LibraryId library_id):
	ICommand(id),
	m_library_id(library_id)
{
}

ICommand::Type UnloadDllCommand::type() const
{
	return Type::UNLOAD_DLL;
}
