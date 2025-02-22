#pragma once
#include "ICommandFactory.hpp"
#include "LibrariesContainer.hpp"
#include "Communicators/ICommunicator.hpp"
#include "Communicators/Protocol/ExecuteCommandsResponse.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Buffer.hpp"

class Liver final
{
public:
	explicit Liver(Event::Ptr quit_event, ICommandFactory::Ptr command_factory, ICommunicator::Ptr communicator);
	~Liver() = default;
	Liver(const Liver&) = delete;
	Liver& operator=(const Liver&) = delete;
	Liver(Liver&&) = delete;
	Liver& operator=(Liver&&) = delete;

private:
	[[nodiscard]] static std::wstring quit_event_name();

	void handle_execute_commands(const ExecuteCommandsResponse& response);
	void execute_commands(const std::vector<ICommand::Ptr>& commands);

public:
	void run();
	[[nodiscard]] static std::unique_ptr<Liver> create(const Buffer& liver_configuration);

private:
	Event::Ptr m_quit_event;
	ICommandFactory::Ptr m_command_factory;
	ICommunicator::Ptr m_communicator;

public:
	LibrariesContainer libraries;
};
