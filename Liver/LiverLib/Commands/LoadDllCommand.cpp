#include "Commands/LoadDllCommand.hpp"

LoadDllCommand::LoadDllCommand(const Id id, const LibrariesContainer::LibraryId library_id, Buffer dll_buffer):
	ICommand(id),
	m_library_id(library_id),
	m_dll_buffer(std::move(dll_buffer))
{
}

ICommand::Type LoadDllCommand::type() const
{
	return Type::LOAD_DLL;
}
