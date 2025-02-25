#include "Communicators/RawCommunicator.hpp"

#include "Communicators/Protocol/ResponseFactory.hpp"

RawCommunicator::RawCommunicator(IInputStream::Ptr input, IOutputStream::Ptr output):
	m_input(std::move(input)),
	m_output(std::move(output))
{
}

IResponse::Ptr RawCommunicator::send(const IRequest::Ptr request)
{
	m_output->write(request->serialize());
	return ResponseFactory::create(m_input);
}

std::unique_ptr<RawCommunicator> RawCommunicator::from_stream(const IIOStream::Ptr& stream)
{
	return std::make_unique<RawCommunicator>(stream, stream);
}
