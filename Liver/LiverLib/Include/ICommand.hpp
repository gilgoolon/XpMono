#pragma once
#include <cstdint>
#include <memory>

class ICommand
{
public:
	using Ptr = std::shared_ptr<ICommand>;
	using Id = uint32_t;

	explicit ICommand() = default;
	virtual ~ICommand() = default;
	ICommand(const ICommand&) = delete;
	ICommand& operator=(const ICommand&) = delete;
	ICommand(ICommand&&) = delete;
	ICommand& operator=(ICommand&&) = delete;

	[[nodiscard]] Id id() const;
};
