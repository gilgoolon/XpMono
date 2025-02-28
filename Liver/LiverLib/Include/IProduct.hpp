#pragma once
#include "Commands/ICommand.hpp"
#include "Interfaces/ISerializable.hpp"

#include <memory>

class IProduct : public ISerializable
{
public:
	using Ptr = std::unique_ptr<IProduct>;
	using Id = uint32_t;

	enum class Type : uint32_t
	{
		ERROR_PRODUCT = 0,
	};

private:
	explicit IProduct(uint32_t id, ICommand::Id command_id);

public:
	explicit IProduct(const ICommand::Ptr& command);
	~IProduct() override = default;
	IProduct(const IProduct&) = delete;
	IProduct& operator=(const IProduct&) = delete;
	IProduct(IProduct&&) = delete;
	IProduct& operator=(IProduct&&) = delete;

	[[nodiscard]] virtual Type type() const = 0;
	[[nodiscard]] virtual Buffer data() const = 0;

	[[nodiscard]] Id id() const;

	[[nodiscard]] Buffer serialize() const final;

private:
	Id m_id;
	ICommand::Id m_command_id;
};
