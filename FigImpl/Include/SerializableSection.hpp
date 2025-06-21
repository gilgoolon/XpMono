#pragma once
#include "Interfaces/ISerializableStruct.hpp"

#include <string>
#include <vector>

struct SerializableSection final
{
	std::wstring name;
	std::vector<ISerializableStruct::Ptr> objects;

	[[nodiscard]] std::wstring serialize() const;
};
