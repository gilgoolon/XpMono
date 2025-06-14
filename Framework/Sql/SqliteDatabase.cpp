#include "Sql/SqliteDatabase.hpp"

#include <SQLiteCpp/SQLiteCpp.h>

SqliteDatabase::SqliteDatabase(const std::wstring& path_or_option):
	m_db(std::make_shared<SQLite::Database>(path_or_option))
{
}

SqliteRowIterator SqliteDatabase::iterate_table(const std::string& table_name) const
{
	return iterate_query("SELECT * FROM " + table_name);
}

SqliteRowIterator SqliteDatabase::iterate_query(const std::string& query) const
{
	return SqliteRowIterator(m_db, query);
}
