#pragma once
#include "Json.hpp"
#include "Filesystem/IFileIterator.hpp"
#include "Handlers/IOperationHandler.hpp"

#include <filesystem>

class DirlistHandler final : public IOperationHandler
{
	explicit DirlistHandler(std::unique_ptr<Event> operation_event, const Json& parameters);

public:
	explicit DirlistHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters);
	~DirlistHandler() override = default;
	DirlistHandler(const DirlistHandler&) = delete;
	DirlistHandler& operator=(const DirlistHandler&) = delete;
	DirlistHandler(DirlistHandler&&) = delete;
	DirlistHandler& operator=(DirlistHandler&&) = delete;

private:
	[[nodiscard]] static std::unique_ptr<IFileIterator> make_iterator(const std::optional<std::filesystem::path>& path,
	                                                                  std::optional<uint32_t> depth);

public:
	void run() override;

private:
	std::optional<std::filesystem::path> m_path;
	std::optional<uint32_t> m_depth;
};
