#pragma once
#include "Commands/ICommand.hpp"
#include "Utils/Buffer.hpp"

struct Command
{
	ICommand::Id id;
	Buffer data;
};
