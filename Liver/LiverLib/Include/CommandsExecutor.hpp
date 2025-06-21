#pragma once
#include "CommandsContainer.hpp"
#include "ProductsContainer.hpp"
#include "Processes/IRunner.hpp"
#include "Synchronization/IWaitable.hpp"

#include <functional>

class CommandsExecutor final : public IRunner
{
public:
	using Consumer = std::function<void(const ICommand::Ptr&)>;

	explicit CommandsExecutor(std::shared_ptr<IWaitable> quit_event,
	                          std::shared_ptr<CommandsContainer> commands_container,
	                          Consumer consumer);
	~CommandsExecutor() override = default;
	CommandsExecutor(const CommandsExecutor&) = delete;
	CommandsExecutor& operator=(const CommandsExecutor&) = delete;
	CommandsExecutor(CommandsExecutor&&) = delete;
	CommandsExecutor& operator=(CommandsExecutor&&) = delete;

	void run() override;

	void consume(const std::vector<ICommand::Ptr>& commands);

private:
	std::shared_ptr<IWaitable> m_quit_event;

	std::shared_ptr<CommandsContainer> m_commands_container;
	Consumer m_consumer;
};
