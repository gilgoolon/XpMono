#include "Include/Liver.hpp"

#include "CommandHandlerFactory.hpp"
#include "ICommandHandler.hpp"
#include "Trace.hpp"
#include "CommandFactories/JsonCommandFactory.hpp"
#include "Commands/ICommand.hpp"
#include "Synchronization/Event.hpp"

Liver::Liver():
	m_quit_event(std::make_shared<Event>(quit_event_name(), Event::Type::MANUAL_RESET)),
	m_command_factory(std::make_unique<JsonCommandFactory>()),
	libraries()
{
}

void Liver::run()
{
	TRACE(L"running liver");

	Buffer command_buffer;
	ICommand::Ptr command = m_command_factory->create(command_buffer);
	const ICommandHandler::Ptr handler = CommandHandlerFactory::create(std::move(command));
	handler->handle(*this);

	TRACE(L"finished liver");
}

std::unique_ptr<Liver> Liver::create([[maybe_unused]] const Buffer& liver_configuration)
{
	return std::make_unique<Liver>();
}

std::wstring Liver::quit_event_name()
{
	static constexpr std::wstring_view QUIT_EVENT_NAME = L"LiverEvent";
	return std::wstring{Event::GLOBAL_NAMESPACE} + std::wstring{QUIT_EVENT_NAME};
}
