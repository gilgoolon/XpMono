#pragma once
#include "Processes/ILibrary.hpp"
#include "Utils/Buffer.hpp"

#include <memory>
#include <string>

struct ApricotLibraryContext;

class ApricotLibrary final : public ILibrary
{
public:
	explicit ApricotLibrary(const Buffer& buffer);
	~ApricotLibrary() override;
	ApricotLibrary(const ApricotLibrary&) = delete;
	ApricotLibrary& operator=(const ApricotLibrary&) = delete;
	ApricotLibrary(ApricotLibrary&&) = delete;
	ApricotLibrary& operator=(ApricotLibrary&&) = delete;

private:
	[[nodiscard]] void* get_exported_procedure(uint16_t ordinal) const override;
	[[nodiscard]] void* get_exported_procedure(const std::string& name) const override;

private:
	std::unique_ptr<ApricotLibraryContext> m_library;
};
