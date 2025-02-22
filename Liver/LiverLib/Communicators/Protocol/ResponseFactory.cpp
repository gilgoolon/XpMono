#include "Communicators/Protocol/ResponseFactory.hpp"

#include "Exception.hpp"
#include "Communicators/Protocol/SendRandomResponse.hpp"

IResponse::Ptr ResponseFactory::create(const IInputStream::Ptr& input)
{
	switch (input->read<IResponse::Type>())
	{
	case IResponse::Type::SEND_RANDOM:
	{
		return std::make_shared<SendRandomResponse>(input->read<uint32_t>());
	}

	default:
	{
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
	}
}
