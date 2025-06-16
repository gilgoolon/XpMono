#include "Sql/InMemorySqliteDatabase.hpp"

#include "Exception.hpp"

#include <sqlite3.h>
#include <SQLiteCpp/Database.h>

static constexpr auto IN_MEMORY_DATABASE = L":memory:";

InMemorySqliteDatabase::InMemorySqliteDatabase(Buffer buffer):
	SqliteDatabase(IN_MEMORY_DATABASE),
	m_buffer(std::move(buffer))
{
	static constexpr auto MAIN_SCHEMA = "main";
	static constexpr unsigned READONLY = SQLITE_DESERIALIZE_READONLY;

	const int result = sqlite3_deserialize(
		m_db->getHandle(),
		MAIN_SCHEMA,
		m_buffer.data(),
		static_cast<int>(m_buffer.size()),
		static_cast<int>(m_buffer.size()),
		READONLY
	);

	if (result != SQLITE_OK)
	{
		throw Exception(ErrorCode::FAILED_SQLITE_DESERIALIZE);
	}
}
