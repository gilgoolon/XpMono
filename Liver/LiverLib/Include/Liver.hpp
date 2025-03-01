#pragma once
#include "ICommandFactory.hpp"
#include "ICommandHandler.hpp"
#include "LibrariesContainer.hpp"
#include "LiverInstance.hpp"
#include "LoadedFigs.hpp"
#include "ProductsContainer.hpp"
#include "Communicators/ICommunicator.hpp"
#include "Communicators/Protocol/ExecuteCommandsResponse.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Buffer.hpp"

class Liver final
{
public:
	explicit Liver(Event::Ptr quit_event,
	               ICommandFactory::Ptr command_factory,
	               ICommunicator::Ptr communicator,
	               Time::Duration iteration_delay);
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
	void register_handlers();
	void register_handler(ICommand::Type type, ICommandHandler::Ptr handler);
	[[nodiscard]] static uint32_t calculate_liver_id();

public:
	void run();
	[[nodiscard]] static std::unique_ptr<Liver> create(const Buffer& liver_configuration);

	[[nodiscard]] uint32_t liver_id() const;

private:
	uint32_t m_liver_id;
	Event::Ptr m_quit_event;
	LiverInstance m_instance;
	ICommandFactory::Ptr m_command_factory;
	ICommunicator::Ptr m_communicator;
	Time::Duration m_iteration_delay;
	ProductsContainer m_products;
	std::shared_ptr<LibrariesContainer> m_libraries;
	std::shared_ptr<LoadedFigs> m_figs;
	std::unordered_map<ICommand::Type, ICommandHandler::Ptr> m_handlers;
};
