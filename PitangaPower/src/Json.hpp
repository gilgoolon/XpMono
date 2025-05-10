#pragma once
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

template <typename T>
std::optional<T> get_optional(const Json& j, const std::string& key)
{
	if (j.contains(key) && !j[key].is_null())
	{
		return j.at(key).get<T>();
	}
	return std::nullopt;
}
