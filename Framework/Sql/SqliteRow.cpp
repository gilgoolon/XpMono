#include "Sql/SqliteRow.hpp"

#include "SQLiteCpp/SQLiteCpp.h"

SqliteRow::SqliteRow(std::shared_ptr<SQLite::Statement> statement) :
	m_statement(std::move(statement))
{
}

template <>
int SqliteRow::get<int>(const int index) const
{
	return m_statement->getColumn(index).getInt();
}

template <>
double SqliteRow::get<double>(const int index) const
{
	return m_statement->getColumn(index).getDouble();
}

template <>
std::string SqliteRow::get<std::string>(const int index) const
{
	return m_statement->getColumn(index).getText();
}

template <>
const char* SqliteRow::get<const char*>(const int index) const
{
	return m_statement->getColumn(index).getText();
}

template <>
bool SqliteRow::get<bool>(const int index) const
{
	return m_statement->getColumn(index).getInt() != 0;
}
