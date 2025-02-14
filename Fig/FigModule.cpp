#include "FigModule.hpp"

#include "FigException.hpp"
#include "FigOperation.hpp"
#include "Processes/DynamicLibrary.hpp"

#include <ApricotLibrary.hpp>

FigModule::FigModule(const Fig::FigId fig_id, std::unique_ptr<ILibrary> library):
	m_library(std::move(library)),
	m_interfaces{},
	m_information{},
	m_fig_quit_event(event_name(fig_id), Event::Type::MANUAL_RESET)
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

FigModule::FigModule(const Fig::FigId fig_id, const Buffer& data):
	FigModule(fig_id, std::make_unique<ApricotLibrary>(data))
{
}

FigModule::FigModule(const Fig::FigId fig_id, const std::filesystem::path& path):
	FigModule(fig_id, std::make_unique<DynamicLibrary>(path))
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
	const Fig::FigCode code = m_interfaces.exeute(
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
	static constexpr uint32_t BUFFER_SIZE = 4096;
	Buffer buffer(BUFFER_SIZE);
	uint32_t size = buffer.size();
	const Fig::FigCode code = m_interfaces.take(id, buffer.data(), &size);
	if (code != Fig::FigCode::SUCCESS)
	{
		throw FigException(ErrorCode::FAILED_FIG_TAKE, code);
	}
	buffer.resize(size);
	return buffer;
}

std::wstring FigModule::event_name(const Fig::FigId fig_id)
{
	return std::wstring{Event::GLOBAL_NAMESPACE} + std::wstring{EVENT_PREFIX} + std::to_wstring(fig_id);
}
