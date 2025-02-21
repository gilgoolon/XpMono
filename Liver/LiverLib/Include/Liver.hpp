#pragma once
#include "CommandHandlerFactory.hpp"
#include "ICommandFactory.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Buffer.hpp"

class Liver final
{
public:
	explicit Liver();
	~Liver() = default;
	Liver(const Liver&) = delete;
	Liver& operator=(const Liver&) = delete;
	Liver(Liver&&) = delete;
	Liver& operator=(Liver&&) = delete;

private:
	void run();

public:
	[[nodiscard]] static std::unique_ptr<Liver> create(const Buffer& liver_configuration);
	static void main(const Buffer& liver_configuration);

	[[nodiscard]] static std::wstring quit_event_name();

private:
	Event::Ptr m_quit_event;
	ICommandFactory::Ptr m_command_factory;
};
