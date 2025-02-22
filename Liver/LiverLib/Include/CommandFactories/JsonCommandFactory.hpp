#pragma once

class JsonCommandFactory final
{
public:
	explicit JsonCommandFactory();
	~JsonCommandFactory();
	JsonCommandFactory(const JsonCommandFactory&) = delete;
	JsonCommandFactory& operator=(const JsonCommandFactory&) = delete;
	JsonCommandFactory(JsonCommandFactory&&) = delete;
	JsonCommandFactory& operator=(JsonCommandFactory&&) = delete;
};
