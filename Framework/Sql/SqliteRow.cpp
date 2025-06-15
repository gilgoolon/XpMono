#include "Sql/SqliteRow.hpp"

#include "SQLiteCpp/SQLiteCpp.h"
#include "Utils/Strings.hpp"

SqliteRow::SqliteRow(std::shared_ptr<SQLite::Statement> statement) :
	m_statement(std::move(statement))
{
}

template <>
int32_t SqliteRow::get<int32_t>(const int index) const
{
	return m_statement->getColumn(index).getInt();
}

template <>
int64_t SqliteRow::get<int64_t>(const int index) const
{
	return m_statement->getColumn(index).getInt64();
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
std::wstring SqliteRow::get<std::wstring>(const int index) const
{
	return Strings::to_wstring(get<std::string>(index));
}

template <>
Buffer SqliteRow::get<Buffer>(const int index) const
{
	const SQLite::Column column = m_statement->getColumn(index);
	const auto data = static_cast<const uint8_t*>(column.getBlob());
	return Buffer{data, data + column.getBytes()};
}

template <>
bool SqliteRow::get<bool>(const int index) const
{
	return m_statement->getColumn(index).getInt() != 0;
}
