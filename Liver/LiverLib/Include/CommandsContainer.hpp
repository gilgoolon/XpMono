#pragma once
#include "IProduct.hpp"
#include "Synchronization/CriticalSection.hpp"

class CommandsContainer final
{
public:
	explicit CommandsContainer() = default;
	~CommandsContainer() = default;
	CommandsContainer(const CommandsContainer&) = delete;
	CommandsContainer& operator=(const CommandsContainer&) = delete;
	CommandsContainer(CommandsContainer&&) = delete;
	CommandsContainer& operator=(CommandsContainer&&) = delete;

	void insert_all(const std::vector<ICommand::Ptr>& commands);
	[[nodiscard]] ICommand::Ptr pop();
	[[nodiscard]] bool has_new() const;

private:
	CriticalSection m_lock;
	std::vector<ICommand::Ptr> m_commands;
};
