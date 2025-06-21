#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>

class ISerializableStruct
{
public:
	using Ptr = std::unique_ptr<ISerializableStruct>;

	explicit ISerializableStruct() = default;
	virtual ~ISerializableStruct() = default;
	ISerializableStruct(const ISerializableStruct&) = delete;
	ISerializableStruct& operator=(const ISerializableStruct&) = delete;
	ISerializableStruct(ISerializableStruct&&) = delete;
	ISerializableStruct& operator=(ISerializableStruct&&) = delete;

	using Fields = std::vector<std::pair<std::wstring, std::optional<std::wstring>>>;

	[[nodiscard]] virtual std::wstring type() const = 0;
	[[nodiscard]] virtual Fields fields() const = 0;

	[[nodiscard]] std::wstring serialize() const;

private:
	[[nodiscard]] static std::wstring format_fields(const Fields& fields);
};
