#pragma once
#include "Sql/SqliteRow.hpp"

#include <memory>
#include <string>

namespace SQLite
{
class Database;
}

class SqliteRowIterator final
{
	explicit SqliteRowIterator(const std::shared_ptr<SQLite::Database>& db, const std::string& query);

public:
	~SqliteRowIterator() = default;
	SqliteRowIterator(const SqliteRowIterator&) = delete;
	SqliteRowIterator& operator=(const SqliteRowIterator&) = delete;
	SqliteRowIterator(SqliteRowIterator&&) = default;
	SqliteRowIterator& operator=(SqliteRowIterator&&) = default;

	[[nodiscard]] bool next();
	[[nodiscard]] SqliteRow current() const;

	friend class SqliteDatabase;

private:
	std::shared_ptr<SQLite::Statement> m_statement;
	bool m_has_row;
};
