#pragma once
#include "FigModule.hpp"
#include "Synchronization/UnmanagedEvent.hpp"

#include <memory>

class FigModule;

class FigOperation
{
	explicit FigOperation(std::shared_ptr<FigModule> module, Fig::OperationId id, HANDLE unmanaged_event);

public:
	~FigOperation() = default;
	FigOperation(const FigOperation&) = delete;
	FigOperation& operator=(const FigOperation&) = delete;
	FigOperation(FigOperation&&) = delete;
	FigOperation& operator=(FigOperation&&) = delete;

	[[nodiscard]] Buffer take();
	[[nodiscard]] FigModule::StatusResult status() const;
	[[nodiscard]] FigModule::StatusResult wait() const;

	friend FigModule;
	friend class FigOperationsFetcher;

private:
	std::shared_ptr<FigModule> m_module;
	Fig::OperationId m_id;
	std::shared_ptr<UnmanagedEvent> m_event;
};
