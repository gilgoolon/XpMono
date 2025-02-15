#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Buffer.hpp"

class GetFileHandler final : public IOperationHandler
{
	explicit GetFileHandler(std::unique_ptr<Event> operation_event, const Json& parameters);

public:
	explicit GetFileHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters);
	~GetFileHandler() override = default;
	GetFileHandler(const GetFileHandler&) = delete;
	GetFileHandler& operator=(const GetFileHandler&) = delete;
	GetFileHandler(GetFileHandler&&) = delete;
	GetFileHandler& operator=(GetFileHandler&&) = delete;

	void run() override;

private:
	std::filesystem::path m_path;
};
