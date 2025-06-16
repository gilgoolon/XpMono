#pragma once
#include "Sql/SqliteRowIterator.hpp"

#include <memory>
#include <string>

class SqliteDatabase
{
public:
	explicit SqliteDatabase(const std::wstring& path_or_option);
	virtual ~SqliteDatabase() = default;
	SqliteDatabase(const SqliteDatabase&) = delete;
	SqliteDatabase& operator=(const SqliteDatabase&) = delete;
	SqliteDatabase(SqliteDatabase&&) = delete;
	SqliteDatabase& operator=(SqliteDatabase&&) = delete;

	[[nodiscard]] SqliteRowIterator iterate_table(const std::string& table_name) const;
	[[nodiscard]] SqliteRowIterator iterate_query(const std::string& query) const;

protected:
	std::shared_ptr<SQLite::Database> m_db;
};
