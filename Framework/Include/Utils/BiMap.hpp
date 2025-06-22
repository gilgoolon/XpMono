#pragma once

#include "Exception.hpp"

#include <initializer_list>
#include <unordered_map>
#include <utility>

namespace std
{
template <>
struct hash<GUID>
{
	std::size_t operator()(const GUID& guid) const noexcept
	{
		const auto p = reinterpret_cast<const uint64_t*>(&guid);
		return std::hash<uint64_t>()(p[0]) ^ std::hash<uint64_t>()(p[1]);
	}
};
}

template <typename Key, typename Value>
class BiMap
{
public:
	BiMap();
	BiMap(std::initializer_list<std::pair<const Key, Value>> list);

	void insert(const Key& key, const Value& value);
	void erase_by_key(const Key& key);
	void erase_by_value(const Value& value);

	const Value& at_key(const Key& key) const;
	const Key& at_value(const Value& value) const;

	bool contains_key(const Key& key) const;
	bool contains_value(const Value& value) const;

	[[nodiscard]] size_t size() const;
	void clear();

	const std::unordered_map<Key, Value>& map() const;
	const std::unordered_map<Value, Key>& inverse_map() const;

private:
	std::unordered_map<Key, Value> m_key_to_value;
	std::unordered_map<Value, Key> m_value_to_key;
};

template <typename Key, typename Value>
BiMap<Key, Value>::BiMap() = default;

template <typename Key, typename Value>
BiMap<Key, Value>::BiMap(std::initializer_list<std::pair<const Key, Value>> list)
{
	for (const auto& [k, v] : list)
	{
		insert(k, v);
	}
}

template <typename Key, typename Value>
void BiMap<Key, Value>::insert(const Key& key, const Value& value)
{
	if (m_key_to_value.contains(key) || m_value_to_key.contains(value))
	{
		throw Exception(ErrorCode::ALREADY_EXISTS);
	}

	m_key_to_value[key] = value;
	m_value_to_key[value] = key;
}

template <typename Key, typename Value>
void BiMap<Key, Value>::erase_by_key(const Key& key)
{
	auto it = m_key_to_value.find(key);
	if (it == m_key_to_value.end())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}

	m_value_to_key.erase(it->second);
	m_key_to_value.erase(it);
}

template <typename Key, typename Value>
void BiMap<Key, Value>::erase_by_value(const Value& value)
{
	auto it = m_value_to_key.find(value);
	if (it == m_value_to_key.end())
	{
		throw Exception(ErrorCode::OUT_OF_BOUNDS);
	}

	m_key_to_value.erase(it->second);
	m_value_to_key.erase(it);
}

template <typename Key, typename Value>
const Value& BiMap<Key, Value>::at_key(const Key& key) const
{
	return m_key_to_value.at(key);
}

template <typename Key, typename Value>
const Key& BiMap<Key, Value>::at_value(const Value& value) const
{
	return m_value_to_key.at(value);
}

template <typename Key, typename Value>
bool BiMap<Key, Value>::contains_key(const Key& key) const
{
	return m_key_to_value.contains(key);
}

template <typename Key, typename Value>
bool BiMap<Key, Value>::contains_value(const Value& value) const
{
	return m_value_to_key.contains(value);
}

template <typename Key, typename Value>
size_t BiMap<Key, Value>::size() const
{
	return m_key_to_value.size();
}

template <typename Key, typename Value>
void BiMap<Key, Value>::clear()
{
	m_key_to_value.clear();
	m_value_to_key.clear();
}

template <typename Key, typename Value>
const std::unordered_map<Key, Value>& BiMap<Key, Value>::map() const
{
	return m_key_to_value;
}

template <typename Key, typename Value>
const std::unordered_map<Value, Key>& BiMap<Key, Value>::inverse_map() const
{
	return m_value_to_key;
}
