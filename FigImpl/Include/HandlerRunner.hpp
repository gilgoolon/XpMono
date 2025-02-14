#pragma once
#include "Handlers/IOperationHandler.hpp"
#include "Processes/IRunner.hpp"

#include <memory>

class HandlerRunner final : public IRunner
{
public:
	explicit HandlerRunner(std::shared_ptr<IOperationHandler> handler);
	~HandlerRunner() override = default;
	HandlerRunner(const HandlerRunner&) = delete;
	HandlerRunner& operator=(const HandlerRunner&) = delete;
	HandlerRunner(HandlerRunner&&) = delete;
	HandlerRunner& operator=(HandlerRunner&&) = delete;

	void run() override;

private:
	std::shared_ptr<IOperationHandler> m_handler;
};
