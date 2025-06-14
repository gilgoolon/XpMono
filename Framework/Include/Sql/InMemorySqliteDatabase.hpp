#pragma once
#include "SqliteDatabase.hpp"
#include "Utils/Buffer.hpp"

class InMemorySqliteDatabase final : public SqliteDatabase
{
public:
	explicit InMemorySqliteDatabase(Buffer buffer);
	~InMemorySqliteDatabase() override = default;
	InMemorySqliteDatabase(const InMemorySqliteDatabase&) = delete;
	InMemorySqliteDatabase& operator=(const InMemorySqliteDatabase&) = delete;
	InMemorySqliteDatabase(InMemorySqliteDatabase&&) = delete;
	InMemorySqliteDatabase& operator=(InMemorySqliteDatabase&&) = delete;

private:
	Buffer m_buffer;
};
