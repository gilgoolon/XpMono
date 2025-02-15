#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"

#include <filesystem>

class DropFileHandler final : public IOperationHandler
{
	explicit DropFileHandler(std::unique_ptr<Event> operation_event,
	                         const Json& parameters);

public:
	explicit DropFileHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters);
	~DropFileHandler() override = default;
	DropFileHandler(const DropFileHandler&) = delete;
	DropFileHandler& operator=(const DropFileHandler&) = delete;
	DropFileHandler(DropFileHandler&&) = delete;
	DropFileHandler& operator=(DropFileHandler&&) = delete;

	void run() override;

private:
	std::filesystem::path m_destination;
	Buffer m_data;
	std::optional<std::filesystem::path> m_metadata_file;
};
