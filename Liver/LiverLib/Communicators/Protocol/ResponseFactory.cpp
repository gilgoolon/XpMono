﻿#include "Communicators/Protocol/ResponseFactory.hpp"

#include "Exception.hpp"
#include "Communicators/Protocol/ExecuteCommandsResponse.hpp"
#include "Communicators/Protocol/KeepAliveResponse.hpp"
#include "Communicators/Protocol/SendRandomResponse.hpp"

IResponse::Ptr ResponseFactory::create(const IInputStream::Ptr& input)
{
	switch (input->read<IResponse::Type>())
	{
	case IResponse::Type::KEEP_ALIVE:
	{
		return std::make_shared<KeepAliveResponse>();
	}

	case IResponse::Type::EXECUTE_COMMANDS:
	{
		const uint32_t total_commands = input->read<uint32_t>();
		std::vector<Command> commands;
		commands.reserve(total_commands);
		for (uint32_t i = 0; i < total_commands; ++i)
		{
			const auto command_id = input->read<ICommand::Id>();
			const Buffer command_data = input->read_sized();
			commands.emplace_back(command_id, command_data);
		}
		return std::make_shared<ExecuteCommandsResponse>(std::move(commands));
	}

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
