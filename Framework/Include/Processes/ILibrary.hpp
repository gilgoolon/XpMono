#pragma once
#include <string>

class ILibrary
{
public:
	explicit ILibrary() = default;
	virtual ~ILibrary() = default;
	ILibrary(const ILibrary&) = delete;
	ILibrary& operator=(const ILibrary&) = delete;
	ILibrary(ILibrary&&) = delete;
	ILibrary& operator=(ILibrary&&) = delete;

private:
	[[nodiscard]] virtual void* get_exported_procedure(uint16_t ordinal) const = 0;
	[[nodiscard]] virtual void* get_exported_procedure(const std::string& name) const = 0;

public:
	template <typename FunctionType, typename... Args>
	auto call(const uint16_t ordinal, Args&&... args) const
	{
		auto func = reinterpret_cast<FunctionType>(get_exported_procedure(ordinal));
		return (*func)(std::forward<Args>(args)...);
	}

	template <typename FunctionType, typename... Args>
	auto call(const std::string& name, Args&&... args) const
	{
		auto func = reinterpret_cast<FunctionType>(get_exported_procedure(name));
		return (*func)(std::forward<Args>(args)...);
	}
};
