#include "Communicators/Protocol/ResponseFactory.hpp"

#include "Exception.hpp"
#include "Communicators/Protocol/ExecuteCommandsResponse.hpp"
#include "Communicators/Protocol/SendRandomResponse.hpp"

IResponse::Ptr ResponseFactory::create(const IInputStream::Ptr& input)
{
	switch (input->read<IResponse::Type>())
	{
	case IResponse::Type::SEND_RANDOM:
	{
		return std::make_shared<SendRandomResponse>(input->read<uint32_t>());
	}

	case IResponse::Type::EXECUTE_COMMANDS:
	{
		const uint32_t total_commands = input->read<uint32_t>();
		std::vector<Buffer> commands;
		commands.reserve(total_commands);
		for (uint32_t i = 0; i < total_commands; ++i)
		{
			commands.emplace_back(input->read_sized());
		}
		return std::make_shared<ExecuteCommandsResponse>(std::move(commands));
	}

	default:
	{
		throw Exception(ErrorCode::UNCOVERED_ENUM_VALUE);
	}
	}
}
