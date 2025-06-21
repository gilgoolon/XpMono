#include "SerializableSection.hpp"

#include "Utils/Strings.hpp"

std::wstring SerializableSection::serialize() const
{
	static constexpr auto SEPARATOR = L"\n";
	std::wstring result = Strings::concat(std::wstring{L"["}, name, std::wstring{L"]"}, std::wstring{SEPARATOR});

	for (const ISerializableStruct::Ptr& object : objects)
	{
		result.append(Strings::concat(object->serialize(), std::wstring{SEPARATOR}));
	}

	return result;
}

std::wstring SerializableSection::serialize_sections(const std::vector<SerializableSection>& sections)
{
	static constexpr auto SEPARATOR = L"\n";
	std::wstring result;

	for (const SerializableSection& section : sections)
	{
		result.append(section.serialize()).append(SEPARATOR);
	}

	return result;
}
