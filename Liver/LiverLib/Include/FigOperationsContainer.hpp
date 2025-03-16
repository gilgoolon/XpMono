#pragma once
#include "FigOperation.hpp"
#include "Commands/ICommand.hpp"
#include "Synchronization/CriticalSection.hpp"
#include "Synchronization/Event.hpp"

#include <functional>

class FigOperationsContainer final
{
	struct CommandLinkedFigOperation final
	{
		std::unique_ptr<FigOperation> fig_operation;
		ICommand::Ptr linked_command;
	};

	using OperationsHandler = std::function<void(std::vector<CommandLinkedFigOperation>&)>;

public:
	explicit FigOperationsContainer();
	~FigOperationsContainer() = default;
	FigOperationsContainer(const FigOperationsContainer&) = delete;
	FigOperationsContainer& operator=(const FigOperationsContainer&) = delete;
	FigOperationsContainer(FigOperationsContainer&&) = delete;
	FigOperationsContainer& operator=(FigOperationsContainer&&) = delete;

	void consume(std::unique_ptr<FigOperation> operation, ICommand::Ptr command);

	void handle_operations(const OperationsHandler& handler);

	friend class FigOperationsFetcher;

private:
	CriticalSection m_lock;
	std::vector<CommandLinkedFigOperation> m_operations;
	Event::Ptr m_notifier;
};
