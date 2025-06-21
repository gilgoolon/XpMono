#include "Handlers/IOperationHandler.hpp"

IOperationHandler::IOperationHandler(std::unique_ptr<Event> operation_event):
	m_lock(),
	m_event(std::move(operation_event)),
	m_products(),
	m_status(Fig::ExecutionStatus::EXECUTING),
	m_specific_code(Fig::FIG_SPECIFIC_CODE_RESERVED)
{
}

Buffer IOperationHandler::take()
{
	const CriticalSection::Acquired acquired = m_lock.acquire();

	if (m_status == Fig::ExecutionStatus::EXECUTING_CAN_TAKE)
	{
		m_status = Fig::ExecutionStatus::EXECUTING;
	}

	Buffer result;
	for (const std::unique_ptr<TypedProduct>& product : m_products)
	{
		const Buffer serialized = product->serialize();
		result.insert(result.end(), serialized.begin(), serialized.end());
	}

	m_products.clear();
	return result;
}

Fig::ExecutionStatus IOperationHandler::status()
{
	return m_status;
}

Fig::FigCode IOperationHandler::fig_code()
{
	return m_fig_code;
}

Fig::FigSpecificCode IOperationHandler::specific_code()
{
	return m_specific_code;
}

void IOperationHandler::error(const Fig::FigCode fig_code, const Fig::FigSpecificCode specific_code)
{
	const auto acquired = m_lock.acquire();
	m_status = Fig::ExecutionStatus::FAILED;
	m_fig_code = fig_code;
	m_specific_code = specific_code;
	m_event->set();
}

void IOperationHandler::finished()
{
	const auto acquired = m_lock.acquire();
	m_status = Fig::ExecutionStatus::FINISHED;
	m_event->set();
}

void IOperationHandler::append(std::unique_ptr<TypedProduct> product)
{
	const auto acquired = m_lock.acquire();
	m_products.push_back(std::move(product));
	m_status = Fig::ExecutionStatus::EXECUTING_CAN_TAKE;
	m_event->set();
}
