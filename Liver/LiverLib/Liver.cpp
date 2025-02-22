#include "Include/Liver.hpp"

#include "CommandHandlerFactory.hpp"
#include "ICommandHandler.hpp"
#include "Trace.hpp"
#include "CommandFactories/JsonCommandFactory.hpp"
#include "Commands/ICommand.hpp"
#include "Communicators/RawCommunicator.hpp"
#include "Filesystem/File.hpp"
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
	TRACE(L"running liver");

	Buffer command_buffer;
	ICommand::Ptr command = m_command_factory->create(command_buffer);
	const ICommandHandler::Ptr handler = CommandHandlerFactory::create(std::move(command));
	handler->handle(*this);

	TRACE(L"finished liver");
}

std::unique_ptr<Liver> Liver::create([[maybe_unused]] const Buffer& liver_configuration)
{
	return std::make_unique<Liver>(
		std::make_shared<Event>(quit_event_name(), Event::Type::MANUAL_RESET),
		std::make_unique<JsonCommandFactory>(),
		RawCommunicator::from_stream(
			std::make_shared<File>(L"protocol.txt", File::Mode::READ_WRITE, File::Disposition::OPEN)
		)
	);
}

std::wstring Liver::quit_event_name()
{
	static constexpr std::wstring_view QUIT_EVENT_NAME = L"LiverEvent";
	return std::wstring{Event::GLOBAL_NAMESPACE} + std::wstring{QUIT_EVENT_NAME};
}
