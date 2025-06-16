#pragma once
#include "External/nlohmann/json.hpp"

#include "Exception.hpp"
#include "Utils/Strings.hpp"

using Json = nlohmann::json;

template <typename T>
std::optional<T> get_optional(const Json& j, const std::string& key)
{
	if (j.contains(key) && j[key].is_null() == false)
	{
		try
		{
			return j.at(key).get<T>();
		}
		catch (const nlohmann::json::exception&)
		{
			throw Exception(ErrorCode::INVALID_ARGUMENT);
		}
	}
	return std::nullopt;
}

inline std::wstring get_wstring(const Json& j)
{
	return Strings::to_wstring(j.get<std::string>());
}
