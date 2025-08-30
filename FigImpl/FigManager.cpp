#include "FigManager.hpp"

#include "Exception.hpp"
#include "FigImplException.hpp"
#include "HandlerRunner.hpp"
#include "Trace.hpp"
#include "Processes/Thread.hpp"
#include "Protections/EntryPointProtector.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Random.hpp"

std::shared_ptr<UnmanagedEvent> FigManager::g_quit_event = nullptr;
std::unordered_map<Fig::OperationId, std::shared_ptr<IOperationHandler>> FigManager::g_operations;
std::unique_ptr<CriticalSection> FigManager::g_operations_lock = nullptr;

Fig::FigCode FigManager::initialize(HANDLE unmanaged_quit_event,
                                    Fig::FigInterfaces* interfaces,
                                    Fig::FigInformation* information)
{
	try
	{
		Protections::EntryPointProtector protector;
		*information = g_information;
		*interfaces = {execute, status, take, free_buffer};
		g_quit_event = std::make_shared<UnmanagedEvent>(unmanaged_quit_event);
		g_operations_lock = std::make_unique<CriticalSection>();
		return Fig::FigCode::SUCCESS;
	}
	catch (const FigImplException& ex)
	{
		TRACE(L"failed with specific code: ", ex.specific_code());
		return ex.code();
	}
	CATCH_AND_TRACE()

	return Fig::FigCode::FAILED_UNKNOWN;
}

static std::unique_ptr<Event> make_operation_event(const Fig::OperationId id)
{
	return std::make_unique<Event>(
		std::wstring{Event::GLOBAL_NAMESPACE} + FigManager::g_name + std::to_wstring(id),
		Event::Type::AUTO_RESET
	);
}

Fig::FigCode FigManager::execute(const Fig::OperationType operation,
                                 const uint8_t* const parameters_buffer,
                                 const uint32_t parameters_buffer_size,
                                 Fig::OperationId* const id,
                                 HANDLE* const operation_event)
{
	try
	{
		Protections::EntryPointProtector protector;
		const uint32_t generated_id = Random::generate<uint32_t>();
		*id = generated_id;
		std::unique_ptr<Event> event = make_operation_event(generated_id);
		*operation_event = event->handle();
		const Buffer raw_parameters = {parameters_buffer, parameters_buffer + parameters_buffer_size};
		std::shared_ptr<IOperationHandler> handler = make_handler(operation, raw_parameters, std::move(event));
		Thread handler_worker(std::make_unique<HandlerRunner>(handler));
		const CriticalSection::Acquired acquired = g_operations_lock->acquire();
		g_operations.emplace(
			generated_id,
			std::move(handler)
		);
		return Fig::FigCode::SUCCESS;
	}
	catch ([[maybe_unused]] const FigImplException& ex)
	{
		TRACE(L"failed with specific code: ", ex.specific_code());
		return ex.code();
	}
	CATCH_AND_TRACE()

	return Fig::FigCode::FAILED_UNKNOWN;
}

Fig::FigCode FigManager::status(const Fig::OperationId id,
                                Fig::ExecutionStatus* const status,
                                Fig::FigSpecificCode* const specific_code)
{
	try
	{
		Protections::EntryPointProtector protector;
		const CriticalSection::Acquired acquired = g_operations_lock->acquire();
		const auto found = g_operations.find(id);
		if (found == g_operations.end())
		{
			return Fig::FigCode::FAILED_INVALID_OPERATION_ID;
		}
		const std::shared_ptr<IOperationHandler>& handler = found->second;
		*status = handler->status();
		*specific_code = handler->specific_code();
		return Fig::FigCode::SUCCESS;
	}
	catch ([[maybe_unused]] const FigImplException& ex)
	{
		TRACE(L"failed with specific code: ", ex.specific_code());
		return ex.code();
	}
	CATCH_AND_TRACE()

	return Fig::FigCode::FAILED_UNKNOWN;
}

Fig::FigCode FigManager::take(const Fig::OperationId id, uint8_t** const buffer, size_t* const buffer_size)
{
	try
	{
		Protections::EntryPointProtector protector;
		const CriticalSection::Acquired acquired = g_operations_lock->acquire();
		const auto found = g_operations.find(id);
		if (found == g_operations.end())
		{
			return Fig::FigCode::FAILED_INVALID_OPERATION_ID;
		}
		const std::shared_ptr<IOperationHandler>& handler = found->second;
		const Buffer result = handler->take();

		auto shared_result = std::make_unique<uint8_t[]>(result.size());
		std::copy_n(result.data(), result.size(), shared_result.get());

		*buffer_size = result.size();
		*buffer = shared_result.get();

		shared_result.release();

		return Fig::FigCode::SUCCESS;
	}
	catch ([[maybe_unused]] const FigImplException& ex)
	{
		TRACE(L"failed with specific code: ", ex.specific_code());
		return ex.code();
	}
	CATCH_AND_TRACE()

	return Fig::FigCode::FAILED_UNKNOWN;
}

Fig::FigCode FigManager::free_buffer(uint8_t* buffer, [[maybe_unused]] uint32_t buffer_size)
{
	try
	{
		Protections::EntryPointProtector protector;
		std::unique_ptr<uint8_t[]> releaser(buffer);
		return Fig::FigCode::SUCCESS;
	}
	catch ([[maybe_unused]] const FigImplException& ex)
	{
		TRACE(L"failed with specific code: ", ex.specific_code());
		return ex.code();
	}
	CATCH_AND_TRACE()

	return Fig::FigCode::FAILED_UNKNOWN;
}

static_assert(
	std::is_same_v<decltype(FigManager::initialize), decltype(Fig::initialize)>,
	"wrong initialize definition"
);
static_assert(std::is_same_v<decltype(FigManager::execute), decltype(Fig::execute)>, "wrong execute definition");
static_assert(std::is_same_v<decltype(FigManager::status), decltype(Fig::status)>, "wrong status definition");
static_assert(std::is_same_v<decltype(FigManager::take), decltype(Fig::take)>, "wrong take definition");
static_assert(
	std::is_same_v<decltype(FigManager::free_buffer), decltype(Fig::free_buffer)>,
	"wrong free_buffer definition"
);
