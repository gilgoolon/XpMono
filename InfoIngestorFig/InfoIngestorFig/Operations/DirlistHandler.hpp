#pragma once
#include "IOperationHandler.hpp"

#include <filesystem>

class DirlistHandler final : public IOperationHandler
{
	explicit DirlistHandler(std::unique_ptr<Event> operation_event, const json& parameters);

public:
	static constexpr Fig::FigId TYPE = 2;

	explicit DirlistHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters);
	~DirlistHandler() override = default;
	DirlistHandler(const DirlistHandler&) = delete;
	DirlistHandler& operator=(const DirlistHandler&) = delete;
	DirlistHandler(DirlistHandler&&) = delete;
	DirlistHandler& operator=(DirlistHandler&&) = delete;

	void run() override;

private:
	std::optional<std::filesystem::path> m_path;
	std::optional<uint32_t> m_depth;
};
