#pragma once
#include "ICommandFactory.hpp"

class JsonCommandFactory final : public ICommandFactory
{
public:
	explicit JsonCommandFactory() = default;
	~JsonCommandFactory() override = default;
	JsonCommandFactory(const JsonCommandFactory&) = delete;
	JsonCommandFactory& operator=(const JsonCommandFactory&) = delete;
	JsonCommandFactory(JsonCommandFactory&&) = delete;
	JsonCommandFactory& operator=(JsonCommandFactory&&) = delete;

	[[nodiscard]] ICommand::Ptr create(const Command& command) override;
};
