#include "FigOperationsContainer.hpp"

#include "Synchronization/Event.hpp"

FigOperationsContainer::FigOperationsContainer():
	m_lock(),
	m_operations(),
	m_notifier(std::make_shared<Event>(Event::Type::AUTO_RESET))
{
}

void FigOperationsContainer::consume(std::unique_ptr<FigOperation> operation, ICommand::Ptr command)
{
	m_notifier->set();

	const auto acquired = m_lock.acquire();

	m_operations.emplace_back(std::move(operation), std::move(command));
}

void FigOperationsContainer::handle_operations(const OperationsHandler& handler)
{
	const auto acquired = m_lock.acquire();

	handler(m_operations);
}
