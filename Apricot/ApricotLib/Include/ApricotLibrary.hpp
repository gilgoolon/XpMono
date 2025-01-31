﻿#pragma once
#include "Utils/Buffer.hpp"

#include <memory>
#include <string>

struct ApricotLibraryContext;

class ApricotLibrary final
{
public:
	explicit ApricotLibrary(const Buffer& buffer);
	~ApricotLibrary();
	ApricotLibrary(const ApricotLibrary&) = delete;
	ApricotLibrary& operator=(const ApricotLibrary&) = delete;
	ApricotLibrary(ApricotLibrary&&) = delete;
	ApricotLibrary& operator=(ApricotLibrary&&) = delete;

private:
	[[nodiscard]] void* get_exported_procedure(uint16_t ordinal) const;
	[[nodiscard]] void* get_exported_procedure(const std::string& name) const;

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

private:
	std::unique_ptr<ApricotLibraryContext> m_library;
};
