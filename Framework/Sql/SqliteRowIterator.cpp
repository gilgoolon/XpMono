#include "Sql/SqliteRowIterator.hpp"

#include "Exception.hpp"

#include <SQLiteCpp/SQLiteCpp.h>

SqliteRowIterator::SqliteRowIterator(const std::shared_ptr<SQLite::Database>& db, const std::string& query):
	m_statement(std::make_shared<SQLite::Statement>(*db, query.c_str())),
	m_has_row(false)
{
}

bool SqliteRowIterator::next()
{
	m_has_row = m_statement->executeStep();
	return m_has_row;
}

SqliteRow SqliteRowIterator::current() const
{
	if (!m_has_row)
	{
		throw Exception(ErrorCode::ILLEGAL_OBJECT_STATE);
	}

	return SqliteRow(m_statement);
}
