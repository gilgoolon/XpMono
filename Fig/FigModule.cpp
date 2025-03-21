﻿#include "FigModule.hpp"

#include "FigException.hpp"
#include "FigOperation.hpp"
#include "Processes/DynamicLibrary.hpp"

FigModule::FigModule(std::unique_ptr<ILibrary> library):
	m_library(std::move(library)),
	m_interfaces{},
	m_information{},
	m_fig_quit_event(Event::Type::MANUAL_RESET)
{
	const Fig::FigCode code = m_library->call<Fig::InitializeFunction>(
		Fig::INITIALIZE_ORDINAL,
		m_fig_quit_event.handle(),
		&m_interfaces,
		&m_information
	);
	if (code != Fig::FigCode::SUCCESS)
	{
		throw FigException(ErrorCode::FAILED_FIG_INITIALIZE, code);
	}
}

FigModule::FigModule(const std::filesystem::path& path):
	FigModule(std::make_unique<DynamicLibrary>(path))
{
}

Fig::FigId FigModule::id() const
{
	return m_information.fig_id;
}

Fig::VersionMinor FigModule::minor() const
{
	return m_information.version_minor;
}

Fig::VersionMajor FigModule::major() const
{
	return m_information.version_major;
}

std::unique_ptr<FigOperation> FigModule::execute(const Fig::OperationType type, const Buffer& parameters)
{
	Fig::OperationId operation_id = 0;
	HANDLE operation_event = nullptr;
	const Fig::FigCode code = m_interfaces.execute(
		type,
		parameters.data(),
		parameters.size(),
		&operation_id,
		&operation_event
	);
	if (code != Fig::FigCode::SUCCESS)
	{
		throw FigException(ErrorCode::FAILED_FIG_EXECUTE, code);
	}
	struct PublicFigOperation final : FigOperation
	{
		explicit PublicFigOperation(std::shared_ptr<FigModule> module,
		                            const Fig::OperationId id,
		                            const HANDLE unmanaged_event) :
			FigOperation(std::move(module), id, unmanaged_event)
		{
		}
	};
	return std::make_unique<PublicFigOperation>(shared_from_this(), operation_id, operation_event);
}

FigModule::StatusResult FigModule::status(const Fig::OperationId id) const
{
	Fig::FigSpecificCode specific_code = Fig::FIG_SPECIFIC_CODE_RESERVED;
	auto status = Fig::ExecutionStatus::FINISHED;
	const Fig::FigCode code = m_interfaces.status(id, &status, &specific_code);
	if (code != Fig::FigCode::SUCCESS)
	{
		throw FigException(ErrorCode::FAILED_FIG_STATUS, code);
	}
	return {status, specific_code};
}

std::vector<uint8_t> FigModule::take(const Fig::OperationId id)
{
	uint8_t* buffer = nullptr;
	uint32_t size = 0;
	const Fig::FigCode code = m_interfaces.take(id, &buffer, &size);
	if (code != Fig::FigCode::SUCCESS)
	{
		throw FigException(ErrorCode::FAILED_FIG_TAKE, code);
	}
	Buffer result = {buffer, buffer + size};
	m_interfaces.free_buffer(buffer, size);
	return result;
}
