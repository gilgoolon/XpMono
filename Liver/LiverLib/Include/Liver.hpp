#pragma once
#include "ICommandFactory.hpp"
#include "LibrariesContainer.hpp"
#include "Communicators/ICommunicator.hpp"
#include "Communicators/Protocol/ExecuteCommandsResponse.hpp"
#include "Networking/WsaInstance.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Buffer.hpp"

class Liver final
{
public:
	explicit Liver(Event::Ptr quit_event,
	               ICommandFactory::Ptr command_factory,
	               ICommunicator::Ptr communicator,
	               Time::Duration iteration_timeout);
	~Liver() = default;
	Liver(const Liver&) = delete;
	Liver& operator=(const Liver&) = delete;
	Liver(Liver&&) = delete;
	Liver& operator=(Liver&&) = delete;

private:
	[[nodiscard]] static std::wstring quit_event_name();

	[[nodiscard]] IRequest::Ptr get_next_request();
	void handle_response(IResponse::Ptr response);

	void handle_execute_commands(const ExecuteCommandsResponse& response);
	void execute_commands(const std::vector<ICommand::Ptr>& commands);

public:
	void run();
	[[nodiscard]] static std::unique_ptr<Liver> create(const Buffer& liver_configuration);

private:
	WsaInstance m_wsa_instance;
	Event::Ptr m_quit_event;
	ICommandFactory::Ptr m_command_factory;
	ICommunicator::Ptr m_communicator;
	Time::Duration m_iteration_timeout;

public:
	LibrariesContainer libraries;
};
