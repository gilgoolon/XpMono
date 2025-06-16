#pragma once
#include <memory>
#include <tuple>

namespace SQLite
{
class Statement;
}

class SqliteRow
{
public:
	explicit SqliteRow(std::shared_ptr<SQLite::Statement> statement);

	template <typename T>
	T get(int index) const;

private:
	std::shared_ptr<SQLite::Statement> m_statement;
};

template <typename Tuple, size_t... Indices>
Tuple as_tuple_impl(const SqliteRow& row, std::index_sequence<Indices...>)
{
	return std::make_tuple(row.get<std::tuple_element_t<Indices, Tuple>>(static_cast<int>(Indices))...);
}

template <typename... Types>
[[nodiscard]] std::tuple<Types...> as_tuple(const SqliteRow& row)
{
	return as_tuple_impl<std::tuple<Types...>>(row, std::index_sequence_for<Types...>{});
}
