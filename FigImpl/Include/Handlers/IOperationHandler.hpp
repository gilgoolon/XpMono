#pragma once
#include "FigApi.hpp"
#include "Products/TypedProduct.hpp"
#include "Synchronization/CriticalSection.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Buffer.hpp"

class IOperationHandler
{
public:
	explicit IOperationHandler(std::unique_ptr<Event> operation_event);
	virtual ~IOperationHandler() = default;
	IOperationHandler(const IOperationHandler&) = delete;
	IOperationHandler& operator=(const IOperationHandler&) = delete;
	IOperationHandler(IOperationHandler&&) = delete;
	IOperationHandler& operator=(IOperationHandler&&) = delete;

	[[nodiscard]] virtual Buffer take();
	[[nodiscard]] virtual Fig::ExecutionStatus status();
	[[nodiscard]] virtual Fig::FigCode fig_code();
	[[nodiscard]] virtual Fig::FigSpecificCode specific_code();

	void error(Fig::FigCode fig_code, Fig::FigSpecificCode specific_code);
	void finished();
	void append(std::unique_ptr<TypedProduct> product);

	virtual void run() = 0;

private:
	CriticalSection m_lock;
	std::unique_ptr<Event> m_event;
	std::vector<std::unique_ptr<TypedProduct>> m_products;
	Fig::ExecutionStatus m_status;
	Fig::FigCode m_fig_code;
	Fig::FigSpecificCode m_specific_code;
};
